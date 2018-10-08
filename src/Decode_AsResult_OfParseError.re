module ResultUtil =
  Decode_ParseError.ResultOf({
    type t = DecodeBase.failure;
    let handle = t => t;
  });

module D =
  DecodeBase.DecodeBase(
    ResultUtil.TransformError,
    ResultUtil.Monad,
    ResultUtil.Alt,
  );

include D;
