module Base = Decode_Base;

[@deprecated "Extending the result type is deprecated"]
module Make = Base.Make;

module ParseError = Decode_ParseError;

[@deprecated "Use Decode.ParseError instead"]
module AsOption = Decode_AsOption;

module AsResult = {
  module OfParseError = Decode_AsResult_OfParseError;
};
