module NonEmptyList = Relude.NonEmpty.List;
module ParseError = Decode_ParseError;

module ResultUtil =
  ParseError.ResultOf({
    type t = ParseError.base;
    let handle = t => t;
  });

include DecodeBase.DecodeBase(ResultUtil.TransformError, ResultUtil.Monad);
