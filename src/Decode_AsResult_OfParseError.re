module NonEmptyList = Relude.NonEmpty.List;

module ResultUtil =
  Decode_ParseError.ResultOf({
    type t = DecodeBase.failure;
    let handle = t => t;
  });

include DecodeBase.DecodeBase(
          ResultUtil.TransformError,
          ResultUtil.Monad,
          ResultUtil.Alt,
        );
