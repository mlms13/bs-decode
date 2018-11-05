module ResultUtil = {
  open BsAbstract;
  open BsAbstract.Interface;

  module NelStr: TYPE with type t = NonEmptyList.t(string) = {
    type t = NonEmptyList.t(string);
  };

  type r('a) = Belt.Result.t('a, NelStr.t);

  let result = Result.result;
  let mapErr = (v, fn) =>
    Result.Bifunctor.bimap(BsAbstract.Functions.id, v, fn);

  module Functor: FUNCTOR with type t('a) = r('a) = Result.Functor(NelStr);

  module Apply: APPLY with type t('a) = r('a) = Result.Apply(NelStr);

  module Applicative: APPLICATIVE with type t('a) = r('a) =
    Result.Applicative(NelStr);

  module Monad: MONAD with type t('a) = r('a) = Result.Monad(NelStr);

  module Alt: ALT with type t('a) = r('a) = Result.Alt(NelStr);

  module Infix = {
    include BsAbstract.Infix.Monad(Monad);
    include BsAbstract.Infix.Alt(Alt);
  };

  module Transform: DecodeBase.TransformError with type t('a) = r('a) = {
    type nonrec t('a) = r('a);
    let pureErr = x => Belt.Result.Error(NonEmptyList.pure(x));
    let mapErr = fn =>
      Result.Bifunctor.bimap(BsAbstract.Functions.id, x =>
        NonEmptyList.map(fn, x)
      );

    let valErr = (v, json) => pureErr(DecodeBase.failureToString(v, json));

    let arrErr = (pos, v) =>
      mapErr(
        x =>
          "While decoding array, at position "
          ++ string_of_int(pos)
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

    let lazyAlt = (res, fn) =>
      switch (res) {
      | Belt.Result.Ok(v) => Belt.Result.Ok(v)
      | Belt.Result.Error(_) => fn()
      };
  };

  let note = failure =>
    BsAbstract.Option.maybe(
      ~f=a => Belt.Result.Ok(a),
      ~default=Belt.Result.Error(failure),
    );

  let recoverWith = a => Alt.alt(_, Applicative.pure(a));
};

module D =
  DecodeBase.DecodeBase(
    ResultUtil.Transform,
    ResultUtil.Monad,
    ResultUtil.Alt,
  );

include D;
