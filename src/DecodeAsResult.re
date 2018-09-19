module ResultTransform: DecodeBase.TransformError with type t('a) = ResultOfDecodeFailure.t('a) = {
  type t('a) = ResultOfDecodeFailure.t('a);
  let err = x => Belt.Result.Error(x);
  let mapErr = ResultOfDecodeFailure.mapErr;

  let valErr = (v, json) => err(DecodeFailure.Val(v, json));
  let arrErr = pos => mapErr(x => DecodeFailure.Arr(NonEmptyList.pure((pos, x))));
  let missingFieldErr = field => err(DecodeFailure.objPure(field, MissingField));
  let objErr = field => mapErr(x => DecodeFailure.objPure(field, InvalidField(x)));
};

module DecodeAsResult =
  DecodeBase.DecodeBase(
    ResultTransform,
    ResultOfDecodeFailure.Monad,
    ResultOfDecodeFailure.Alt
  );

include DecodeAsResult;
