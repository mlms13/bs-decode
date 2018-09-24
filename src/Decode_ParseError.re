type t('a) =
  | Val('a, Js.Json.t)
  | Arr(NonEmptyList.t((int, t('a))))
  | Obj(NonEmptyList.t((string, objError('a))))

and objError('a) =
  | MissingField
  | InvalidField(t('a));

let arrPure = (pos, err) =>
  Arr(NonEmptyList.pure((pos, err)));

let objPure = (field, err) =>
  Obj(NonEmptyList.pure((field, err)));

/*
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) => switch (a, b) {
| (Arr(nela), Arr(nelb)) => Arr(NonEmptyList.append(nela, nelb))
| (Obj(nela), Obj(nelb)) => Obj(NonEmptyList.append(nela, nelb))
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
  let spaces = indent => String.make(indent * 4, ' ');

  let msg = switch v {
  | Val(x, json) => innerToString(x, json)
  | Arr(nel) =>
    let childMessages = nel |> NonEmptyList.map(((i, err)) =>
      toDebugString(~level = level + 1, ~pre = "At position " ++ string_of_int(i) ++ ": ", innerToString, err)
    ) |> NonEmptyList.toT |> Array.of_list |> Js.Array.joinWith("\n");

    "Failed to decode array:\n" ++ childMessages;

  | Obj(nel) =>
    let childMessages = nel |> NonEmptyList.map(((field, err)) => {
      let fieldStr = "\"" ++ field ++ "\"";
      switch err {
      | MissingField => spaces(level + 1) ++ "Field " ++ fieldStr ++ " is required, but was not present"
      | InvalidField(err) => toDebugString(~level = level + 1, ~pre = "Field " ++ fieldStr ++ " had an invalid value: ", innerToString, err)
      };
    }) |> NonEmptyList.toT |> Array.of_list |> Js.Array.joinWith("\n");

    "Failed to decode object:\n" ++ childMessages;
  };

  spaces(level) ++ pre ++ msg;
};

module type ValError = {
  type t;
  let handle: DecodeBase.failure => t
};

module ResultOf = (T: ValError) => {
  module Result = Belt.Result;
  open BsAbstract.Interface;

  type r('a) = Result.t('a, t(T.t));
  let result = BsAbstract.Result.result;
  let mapErr = (v, fn) => BsAbstract.Result.Bifunctor.bimap(BsAbstract.Functions.id, v, fn);

  module Functor: FUNCTOR with type t('a) = r('a) = {
    type t('a) = r('a);
    let map = (f, v) => Result.map(v, f);
  };

  module Apply: APPLY with type t('a) = Functor.t('a) = {
    include Functor;
    let apply = (f, v) => switch (f, v) {
    | (Result.Ok(fn), Result.Ok(a)) => Result.Ok(fn(a))
    | (Result.Ok(_), Result.Error(x)) => Result.Error(x)
    | (Result.Error(x), Result.Ok(_)) => Result.Error(x)
    | (Result.Error(fnx), Result.Error(ax)) =>
      Result.Error(combine(fnx, ax))
    };
  };

  module Applicative: APPLICATIVE with type t('a) = Functor.t('a) = {
    include Apply;
    let pure = v => Result.Ok(v);
  };

  module Monad: MONAD with type t('a) = Functor.t('a) = {
    include Applicative;
    let flat_map = Result.flatMap;
  };

  module Alt: ALT with type t('a) = Functor.t('a) = {
    include Functor;
    let alt = (a, b) => result(_ => a, _ => b, a);
  };

  module Infix = {
    include BsAbstract.Infix.Monad(Monad);
    include BsAbstract.Infix.Alt(Alt);
  };

  module TransformError: DecodeBase.TransformError with type t('a) = r('a) = {
    type t('a) = r('a);
    let err = x => Result.Error(x);

    let valErr = (v, json) => err(Val(T.handle(v), json));
    let arrErr = pos => mapErr(x => arrPure(pos, x));
    let missingFieldErr = field => err(objPure(field, MissingField));
    let objErr = field => mapErr(x => objPure(field, InvalidField(x)));
  };

  let note = (failure) => BsAbstract.Option.maybe(
    ~f = a => Result.Ok(a),
    ~default = Result.Error(failure)
  );

  let recoverWith = a => Alt.alt(_, Applicative.pure(a));
};
