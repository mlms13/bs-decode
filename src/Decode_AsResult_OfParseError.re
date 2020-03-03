module NonEmptyList = Relude.NonEmpty.List;
module ParseError = Decode_ParseError;

module Result =
  ParseError.ResultOf({
    type t = ParseError.base;
    let handle = t => t;
  });

include DecodeBase.DecodeBase(Result.TransformError, Result);
