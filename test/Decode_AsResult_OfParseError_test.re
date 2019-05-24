open Jest;
open Expect;
open Relude.Globals;

module Decode = Decode.AsResult.OfParseError;
module Sample = Decode_TestSampleData;
module Err = Decode_ParseError;

describe("Simple decoders", () => {
  test("boolean", () =>
    expect(Decode.boolean(Sample.jsonNull))
    |> toEqual(Result.error(Err.Val(`ExpectedBoolean, Sample.jsonNull)))
  );

  test("string", () =>
    expect(Decode.string(Sample.jsonNull))
    |> toEqual(Result.error(Err.Val(`ExpectedString, Sample.jsonNull)))
  );

  test("floatFromNumber", () =>
    expect(Decode.floatFromNumber(Sample.jsonString))
    |> toEqual(Result.error(Err.Val(`ExpectedNumber, Sample.jsonString)))
  );

  test("intFromNumber", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat))
    |> toEqual(Result.error(Err.Val(`ExpectedInt, Sample.jsonFloat)))
  );

  test("date", () =>
    expect(Decode.date(Sample.jsonString))
    |> toEqual(Result.error(Err.Val(`ExpectedValidDate, Sample.jsonString)))
  );
});
