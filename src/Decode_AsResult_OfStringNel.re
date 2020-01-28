open Relude.Globals;

// we are intentionally aliasing this module here and including the alias in the
// rei to make it easy for users to interact with NonEmptyLists without needing
// to use Relude directly
module NonEmptyList = NonEmpty.List;

module ResultUtil = {
  open BsAbstract.Interface;
  module ParseError = Decode_ParseError;

  module Monad: MONAD with type t('a) = result('a, NonEmptyList.t(string)) = {
    type t('a) = result('a, NonEmptyList.t(string));
    let map = Result.map;
    let apply = (a, b) =>
      switch (a, b) {
      | (Ok(f), Ok(v)) => Ok(f(v))
      | (Ok(_), Error(v)) => Error(v)
      | (Error(v), Ok(_)) => Error(v)
      | (Error(xa), Error(xb)) => Error(NonEmptyList.concat(xa, xb))
      };
    let pure = Result.pure;
    let flat_map = Result.bind;
  };

  module Transform:
    ParseError.TransformError with
      type t('a) = result('a, NonEmptyList.t(string)) = {
    type t('a) = result('a, NonEmptyList.t(string));

    let pureErr = NonEmptyList.pure >> Result.error;
    let mapErr = fn => Result.bimap(id, NonEmptyList.map(fn));
    let valErr = (v, json) => pureErr(ParseError.failureToString(v, json));

    let arrErr = (pos, v) =>
      mapErr(
        x =>
          "While decoding array, at position "
          ++ String.fromInt(pos)
          ++ ": "
          ++ x,
        v,
      );

    let missingFieldErr = field =>
      pureErr("Object field \"" ++ field ++ "\" was missing");

    let objErr = field =>
      mapErr(x =>
        "While decoding object, for field \"" ++ field ++ "\": " ++ x
      );

    let lazyAlt = (res, fn) => Result.fold(_ => fn(), Result.ok, res);
  };
};

include DecodeBase.DecodeBase(ResultUtil.Transform, ResultUtil.Monad);
