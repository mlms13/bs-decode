module R = Decode_ParseError.ResultOf({
  type t = DecodeBase.failure;
  let handle = t => t;
});

module D = DecodeBase.DecodeBase(
  R.TransformError,
  R.Monad,
  R.Alt
);

include D;
