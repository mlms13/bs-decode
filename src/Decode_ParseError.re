open Relude.Globals;

type t('a) =
  | Val('a, Js.Json.t)
  | TriedMultiple(NonEmpty.List.t(t('a)))
  | Arr(NonEmpty.List.t((int, t('a))))
  | Obj(NonEmpty.List.t((string, objError('a))))
and objError('a) =
  | MissingField
  | InvalidField(t('a));

type failure = t(DecodeBase.failure);

let arrPure = (pos, err) => Arr(NonEmpty.List.pure((pos, err)));
let objPure = (field, err) => Obj(NonEmpty.List.pure((field, err)));

/*
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) =>
  switch (a, b) {
  | (Arr(xs), Arr(ys)) => Arr(NonEmpty.List.concat(xs, ys))
  | (Obj(xs), Obj(ys)) => Obj(NonEmpty.List.concat(xs, ys))
  | (TriedMultiple(xs), x) =>
    TriedMultiple(NonEmpty.List.(concat(xs, pure(x))))
  | (Val(_), _)
  | (Arr(_), _)
  | (Obj(_), _) => a
  };

let makeTriedMultiple =
  fun
  | TriedMultiple(_) as x => x
  | Val(_) as err => TriedMultiple(NonEmpty.List.pure(err))
  | Arr(_) as err => TriedMultiple(NonEmpty.List.pure(err))
  | Obj(_) as err => TriedMultiple(NonEmpty.List.pure(err));

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
    | TriedMultiple(xs) =>
      let childMessages =
        xs
        |> NonEmpty.List.map(toDebugString(~level=level + 1, innerToString))
        |> NonEmpty.List.toSequence
        |> List.String.joinWith("\n");

      "Attempted multiple decoders, which all failed:\n" ++ childMessages;

    | Arr(xs) =>
      let childMessages =
        xs
        |> NonEmpty.List.map(((i, err)) =>
             toDebugString(
               ~level=level + 1,
               ~pre="At position " ++ string_of_int(i) ++ ": ",
               innerToString,
               err,
             )
           )
        |> NonEmpty.List.toSequence
        |> List.String.joinWith("\n");

      "Failed to decode array:\n" ++ childMessages;

    | Obj(nel) =>
      let childMessages =
        nel
        |> NonEmpty.List.map(((field, err)) => {
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
        |> NonEmpty.List.toSequence
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

module ResultOf = (Err: ValError) => {
  open BsAbstract.Interface;

  type r('a) = Result.t('a, t(Err.t));

  module Monad: MONAD with type t('a) = r('a) = {
    type t('a) = r('a);
    let map = Result.map;
    let apply = (f, v) =>
      switch (f, v) {
      | (Belt.Result.Ok(fn), Belt.Result.Ok(a)) => Result.ok(fn(a))
      | (Ok(_), Error(_) as err) => err
      | (Error(_) as err, Ok(_)) => err
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

    let valErr = (v, json) => Result.error(Val(Err.handle(v), json));
    let arrErr = pos => Result.mapError(arrPure(pos));
    let missingFieldErr = field =>
      Result.error(objPure(field, MissingField));
    let objErr = field =>
      Result.mapError(x => objPure(field, InvalidField(x)));
    let lazyAlt = (res, fn) =>
      switch (res) {
      | Belt.Result.Ok(_) as ok => ok
      | Error(x) =>
        let err = makeTriedMultiple(x);
        Result.mapError(combine(err), fn());
      };
  };
};
