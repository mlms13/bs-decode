module ResultOfStringNel = {
  module NelStr: BsAbstract.Interface.TYPE with type t = NonEmptyList.t(string) = { type t = NonEmptyList.t(string) };

  type x('a) = Belt.Result.t('a, NelStr.t);

  module Monad: BsAbstract.Interface.MONAD with type t('a) = x('a) = BsAbstract.Result.Monad(NelStr);
  module Alt: BsAbstract.Interface.ALT with type t('a) = x('a) = BsAbstract.Result.Alt(NelStr);

  module Transform: DecodeBase.TransformError with type t('a) = x('a) = {
    type t('a) = Belt.Result.t('a, NelStr.t);
    let mapErr = (fn) => BsAbstract.Result.Bifunctor.bimap(BsAbstract.Functions.id, x => NonEmptyList.map(fn, x));
    let transform = (v): t('a) => Belt.Result.Error(DecodeFailure.toNelString(v));

    let arrErr = (pos, v) =>
      mapErr(x => "While decoding array, at position " ++ string_of_int(pos) ++ ": " ++ x, v);

    let objErr = field =>
      mapErr(x => "While decoding object, for field \"" ++ field ++ "\": " ++ x);
  };
};

module DecodeAsStringNel = DecodeBase.DecodeBase(
  ResultOfStringNel.Transform,
  ResultOfStringNel.Monad,
  ResultOfStringNel.Alt
);
