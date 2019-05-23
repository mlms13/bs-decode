open Jest;
open Expect;

module Decode = Decode.AsOption;
module Sample = Decode_TestSampleData;

describe("Simple decoders", () => {
  test("Decode bool (success)", () =>
    expect(Decode.boolean(Sample.jsonBool))
    |> toEqual(Some(Sample.valBool))
  );

  test("Decode bool (fails on null)", () =>
    expect(Decode.boolean(Sample.jsonNull)) |> toEqual(None)
  );

  test("Decode bool (fails on int)", () =>
    expect(Decode.boolean(Sample.jsonInt)) |> toEqual(None)
  );

  test("Decode bool (fails on string)", () =>
    expect(Decode.boolean(Sample.jsonStringTrue)) |> toEqual(None)
  );

  test("Decode string (success)", () =>
    expect(Decode.string(Sample.jsonString))
    |> toEqual(Some(Sample.valString))
  );

  test("Decode string (fails on float)", () =>
    expect(Decode.string(Sample.jsonFloat)) |> toEqual(None)
  );

  test("Decode float (success)", () =>
    expect(Decode.floatFromNumber(Sample.jsonFloat))
    |> toEqual(Some(Sample.valFloat))
  );

  test("Decode float (fails on string)", () =>
    expect(Decode.floatFromNumber(Sample.jsonString)) |> toEqual(None)
  );

  test("Decode int (success)", () =>
    expect(Decode.intFromNumber(Sample.jsonInt))
    |> toEqual(Some(Sample.valInt))
  );

  test("Decode int (success on zero)", () =>
    expect(Decode.intFromNumber(Sample.jsonIntZero))
    |> toEqual(Some(Sample.valIntZero))
  );

  test("Decode int (fails on string)", () =>
    expect(Decode.intFromNumber(Sample.jsonString)) |> toEqual(None)
  );

  test("Decode int (fails on float)", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat)) |> toEqual(None)
  );

  test("Decode date (succeeds on valid string)", () =>
    expect(Decode.date(Sample.jsonDateString))
    |> toEqual(Some(Sample.valDateString))
  );

  test("Decode date (succeeds on valid number)", () =>
    expect(Decode.date(Sample.jsonDateNumber))
    |> toEqual(Some(Sample.valDateNumber))
  );

  test("Decode date (fails on invalid string)", () =>
    expect(Decode.date(Sample.jsonString)) |> toEqual(None)
  );

  test("Decode date (fails on invalid number)", () =>
    expect(Decode.date(Js.Json.number(Js.Float._NaN))) |> toEqual(None)
  );

  test("Decode date (fails on null)", () =>
    expect(Decode.date(Sample.jsonNull)) |> toEqual(None)
  );
});
