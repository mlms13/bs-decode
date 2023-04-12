module Base = Decode_Base;

[@deprecated "Extending the result type is deprecated"]
module Make = Base.Make;

module ParseError = Decode_ParseError;

module AsResult = {
  module OfParseError = Decode_AsResult_OfParseError;
};
