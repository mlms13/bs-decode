module ParseError = Decode_ParseError;

module AsOption = Decode_AsOption;
module AsResult = {
  module OfParseError = Decode_AsResult_OfParseError;
  module OfStringNel = Decode_AsResult_OfStringNel;
};
