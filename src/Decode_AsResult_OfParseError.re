module NonEmptyList = Relude.NonEmpty.List;
module ParseError = Decode_ParseError;

module ResultUtil =
  ParseError.ResultOf({
    type t = DecodeBase.failure;
    let handle = t => t;
  });

include DecodeBase.DecodeBase(ResultUtil.TransformError, ResultUtil.Monad);
