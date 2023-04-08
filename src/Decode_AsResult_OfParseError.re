module NonEmptyList = Relude.NonEmpty.List;
module ParseError = Decode_ParseError;

[@ocaml.warning "-3"]
module Result =
  ParseError.ResultOf({
    type t = ParseError.base;
    let handle = t => t;
  });

include Decode_Base.Make(Result.TransformError, Result);
