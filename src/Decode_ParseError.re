open Relude.Globals;
module Nel = Relude.NonEmpty.List;

type t('a) =
  | Val('a, Js.Json.t)
  | Arr(Nel.t((int, t('a))))
  | Obj(Nel.t((string, objError('a))))
and objError('a) =
  | MissingField
  | InvalidField(t('a));

type failure = t(DecodeBase.failure);

let rec map = (fn, v) =>
  switch (v) {
  | Val(a, json) => Val(fn(a), json)
  | Arr(nel) => Arr(Nel.map(((i, a)) => (i, map(fn, a)), nel))
  | Obj(nel) => Obj(Nel.map(((f, o)) => (f, mapObj(fn, o)), nel))
  }
and mapObj = (fn, obj) =>
  switch (obj) {
  | MissingField => MissingField
  | InvalidField(a) => InvalidField(map(fn, a))
  };

let arrPure = (pos, err) => Arr(Nel.pure((pos, err)));
let objPure = (field, err) => Obj(Nel.pure((field, err)));

/*
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) =>
  switch (a, b) {
  | (Arr(nela), Arr(nelb)) => Arr(Nel.concat(nela, nelb))
  | (Obj(nela), Obj(nelb)) => Obj(Nel.concat(nela, nelb))
  /* | (Val(a, jsona), Val(b, jsonb)) => f((a, jsona), (b, jsonb)) */
  | _ => a
  };

/**
 * Traverse the tree of errors and produce properly-indented error strings:
 *
 * Failed to decode array:
 *     At position 0: Expected string but found 3.5
 *
 * Failed to decode array:
 *     At position 3: Failed to decode object:
 *         Field "foo" is required but was not present
 *         Field "bar" had an invalid value: Failed to decode array:
 *             At position 0: Expected int but found []
 */
let rec toDebugString = (~level=0, ~pre="", innerToString, v) => {
  let spaces = indent => String.repeat(indent * 4, " ");

  let msg =
    switch (v) {
    | Val(x, json) => innerToString(x, json)
    | Arr(nel) =>
      let childMessages =
        nel
        |> Nel.map(((i, err)) =>
             toDebugString(
               ~level=level + 1,
               ~pre="At position " ++ string_of_int(i) ++ ": ",
               innerToString,
               err,
             )
           )
        |> Nel.toSequence
        |> List.String.joinWith("\n");

      "Failed to decode array:\n" ++ childMessages;

    | Obj(nel) =>
      let childMessages =
        nel
        |> Nel.map(((field, err)) => {
             let fieldStr = "\"" ++ field ++ "\"";
             switch (err) {
             | MissingField =>
               spaces(level + 1)
               ++ "Field "
               ++ fieldStr
               ++ " is required, but was not present"
             | InvalidField(err) =>
               toDebugString(
                 ~level=level + 1,
                 ~pre="Field " ++ fieldStr ++ " had an invalid value: ",
                 innerToString,
                 err,
               )
             };
           })
        |> Nel.toSequence
        |> List.String.joinWith("\n");

      "Failed to decode object:\n" ++ childMessages;
    };

  spaces(level) ++ pre ++ msg;
};

let failureToDebugString = err =>
  toDebugString(DecodeBase.failureToString, err);

module type ValError = {
  type t;
  let handle: DecodeBase.failure => t;
};

module ResultOf = (T: ValError) => {
  open BsAbstract.Interface;

  type r('a) = Result.t('a, t(T.t));
  let mapErr = (v, fn) => Result.bimap(id, v, fn);
  let fromFailure = err => map(T.handle, err);
  let fromFailureResult = err => mapErr(fromFailure, err);

  module Monad: MONAD with type t('a) = r('a) = {
    type t('a) = r('a);
    let map = Result.map;
    let apply = (f, v) =>
      switch (f, v) {
      | (Belt.Result.Ok(fn), Belt.Result.Ok(a)) => Result.ok(fn(a))
      | (Ok(_), Error(x)) => Result.error(x)
      | (Error(x), Ok(_)) => Result.error(x)
      | (Error(fnx), Error(ax)) => Result.error(combine(fnx, ax))
      };
    let pure = Result.pure;
    let flat_map = Result.bind;
  };

  module Alt: ALT with type t('a) = r('a) = {
    include Monad;
    let alt = Result.alt;
  };

  module TransformError: DecodeBase.TransformError with type t('a) = r('a) = {
    type t('a) = r('a);

    let valErr = (v, json) => Result.error(Val(T.handle(v), json));
    let arrErr = pos => mapErr(x => arrPure(pos, x));
    let missingFieldErr = field =>
      Result.error(objPure(field, MissingField));
    let objErr = field => mapErr(x => objPure(field, InvalidField(x)));
    let lazyAlt = (res, fn) =>
      switch (res) {
      | Belt.Result.Ok(v) => Belt.Result.Ok(v)
      | Belt.Result.Error(_) => fn()
      };
  };
};
