module ResultOfStringNel = {
  module NelStr: BsAbstract.Interface.TYPE with type t = NonEmptyList.t(string) = { type t = NonEmptyList.t(string) };

  type x('a) = Belt.Result.t('a, NelStr.t);

  module Monad: BsAbstract.Interface.MONAD with type t('a) = x('a) = BsAbstract.Result.Monad(NelStr);
  module Alt: BsAbstract.Interface.ALT with type t('a) = x('a) = BsAbstract.Result.Alt(NelStr);

  module Transform: DecodeBase.TransformError with type t('a) = x('a) = {
    type t('a) = Belt.Result.t('a, NelStr.t);
    let pureErr = x => Belt.Result.Error(NonEmptyList.pure(x));
    let mapErr = (fn) => BsAbstract.Result.Bifunctor.bimap(BsAbstract.Functions.id, x => NonEmptyList.map(fn, x));

    let valErr = (v, json) => pureErr(DecodeBase.failureToString(v, json));

    let arrErr = (pos, v) =>
      mapErr(x => "While decoding array, at position " ++ string_of_int(pos) ++ ": " ++ x, v);

    let missingFieldErr = field => pureErr("Object field \"" ++ field ++ "\" was missing");

    let objErr = field =>
      mapErr(x => "While decoding object, for field \"" ++ field ++ "\": " ++ x);
  };
};

module DecodeAsStringNel = DecodeBase.DecodeBase(
  ResultOfStringNel.Transform,
  ResultOfStringNel.Monad,
  ResultOfStringNel.Alt
);
