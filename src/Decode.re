module Base = Decode_Base;

[@deprecated "Extending the result type is deprecated"]
module Make = Base.Make;

module ParseError = Decode_ParseError;

module AsOption = Decode_AsOption;
module AsResult = {
  module OfParseError = Decode_AsResult_OfParseError;

  [@deprecated "Use Decode.AsResult.OfParseError instead"]
  module OfStringNel = Decode_AsResult_OfStringNel;
};
