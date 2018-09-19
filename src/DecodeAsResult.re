module ResultTransform: DecodeBase.TransformError with type t('a) = Belt.Result.t('a, DecodeFailure.t) = {
  type t('a) = Belt.Result.t('a, DecodeFailure.t);
  let transform = v => Belt.Result.Error(v);

  /* TODO: replace with ResultOfDecodeFailure.mapErr */
  let mapErr = (fn, v) => switch v {
  | Belt.Result.Ok(a) => Belt.Result.Ok(a)
  | Belt.Result.Error(x) => Belt.Result.Error(fn(x))
  };

  let arrErr = (pos) => mapErr(x => DecodeFailure.Arr(NonEmptyList.pure((pos, x))));
  let objErr = (field) => mapErr(x => DecodeFailure.objPure(field, InvalidField(x)));
};

module DecodeAsResult =
  DecodeBase.DecodeBase(
    ResultTransform,
    ResultOfDecodeFailure.Monad,
    ResultOfDecodeFailure.Alt
  );

include DecodeAsResult;
