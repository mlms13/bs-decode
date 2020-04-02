module ParseError = Decode_ParseError;

module Result =
  ParseError.ResultOf({
    type t = ParseError.base;
    let handle = t => t;
  });

include Decode_Base.Make(Result.TransformError, Result);
