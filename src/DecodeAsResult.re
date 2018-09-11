module ResultTransform: DecodeBase.TransformError with type t('a) = Belt.Result.t('a, DecodeFailure.t) = {
  type t('a) = Belt.Result.t('a, DecodeFailure.t);
  let transform = v => Belt.Result.Error(v);
  let mapErr = (fn, v) => switch v {
  | Belt.Result.Ok(a) => Belt.Result.Ok(a)
  | Belt.Result.Error(x) => Belt.Result.Error(fn(x))
  };
};
module DecodeAsResult = DecodeBase.DecodeBase(
  ResultTransform,
  ResultDecodeError.Monad,
  ResultDecodeError.Alt
);

include DecodeAsResult;
