open Jest;
open Expect;

module Decode = Decode.AsResult.OfParseError;
module Sample = Decode_TestSampleData;
module Err = Decode_ParseError;
module Result = Belt.Result;

describe("Simple decoders", () => {
  test("boolean", () =>
    expect(Decode.boolean(Sample.jsonNull))
    |> toEqual(Result.Error(Err.Val(`ExpectedBoolean, Sample.jsonNull)))
  );

  test("string", () =>
    expect(Decode.string(Sample.jsonNull))
    |> toEqual(Result.Error(Err.Val(`ExpectedString, Sample.jsonNull)))
  );

  test("floatFromNumber", () =>
    expect(Decode.floatFromNumber(Sample.jsonString))
    |> toEqual(Result.Error(Err.Val(`ExpectedNumber, Sample.jsonString)))
  );

  test("intFromNumber", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat))
    |> toEqual(Result.Error(Err.Val(`ExpectedInt, Sample.jsonFloat)))
  );

  test("date", () =>
    expect(Decode.date(Sample.jsonString))
    |> toEqual(Result.Error(Err.Val(`ExpectedValidDate, Sample.jsonString)))
  );
});
