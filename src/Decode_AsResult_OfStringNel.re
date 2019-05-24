open Relude.Globals;

// we are intentionally aliasing this module here and including the alis in the
// rei to make it easy for users to interact with NonEmptyLists without needing
// to use Relude directly
module NonEmptyList = NonEmpty.List;

module ResultUtil = {
  open BsAbstract.Interface;
  type stringNel = NonEmptyList.t(string);

  module Monad: MONAD with type t('a) = Result.t('a, stringNel) = {
    type t('a) = Result.t('a, stringNel);
    let map = Result.map;
    let apply = (a, b) =>
      switch (a, b) {
      | (Belt.Result.Ok(f), Belt.Result.Ok(v)) => Belt.Result.Ok(f(v))
      | (Ok(_), Error(v)) => Error(v)
      | (Error(v), Ok(_)) => Error(v)
      | (Error(xa), Error(xb)) => Error(NonEmpty.List.concat(xa, xb))
      };
    let pure = Result.pure;
    let flat_map = Result.bind;
  };

  module Alt: ALT with type t('a) = Result.t('a, stringNel) = {
    type t('a) = Result.t('a, stringNel);
    let map = Monad.map;
    let alt = Result.alt;
  };

  module Transform:
    DecodeBase.TransformError with type t('a) = Result.t('a, stringNel) = {
    type t('a) = Result.t('a, stringNel);

    let pureErr = NonEmptyList.pure >> Result.error;
    let mapErr = fn => Result.bimap(id, NonEmptyList.map(fn));
    let valErr = (v, json) => pureErr(DecodeBase.failureToString(v, json));

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

include DecodeBase.DecodeBase(
          ResultUtil.Transform,
          ResultUtil.Monad,
          ResultUtil.Alt,
        );
