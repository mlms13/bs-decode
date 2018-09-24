module R = DecodeFailure.AsResult({
  type t = DecodeBase.failure;
  let handle = t => t;
});

module DecodeAsResult = DecodeBase.DecodeBase(
  R.TransformError,
  R.Monad,
  R.Alt
);

include DecodeAsResult;
