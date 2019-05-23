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

  test("Decode int (succeeds on zero)", () =>
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
  // TODO: variantFrom[Json|String|Int]
});

describe("Nested decoders", () => {
  test("Decode optional float (succeeds on null)", () =>
    expect(Decode.(optional(floatFromNumber, Sample.jsonNull)))
    |> toEqual(Some(None))
  );

  test("Decode optional float (succeeds on float)", () =>
    expect(Decode.(optional(floatFromNumber, Sample.jsonFloat)))
    |> toEqual(Some(Some(Sample.valFloat)))
  );

  test("Decode optional float (fails on bool)", () =>
    expect(Decode.(optional(floatFromNumber, Sample.jsonBool)))
    |> toEqual(None)
  );

  // TODO: test `fallback` here when it stops being field-specific

  test("Decode array (succeeds)", () =>
    expect(Decode.(array(string, Sample.jsonArrayString)))
    |> toEqual(Some(Sample.valArrayString))
  );

  test("Decode array (succeeds on empty)", () =>
    expect(Decode.(array(string, Sample.jsonArrayEmpty)))
    |> toEqual(Some(Sample.valArrayEmpty))
  );

  test("Decode array (fails on non-array)", () =>
    expect(Decode.(array(string, Sample.jsonString))) |> toEqual(None)
  );

  test("Decode array (fails on invalid inner data)", () =>
    expect(Decode.(array(boolean, Sample.jsonArrayString))) |> toEqual(None)
  );

  test("Decode list (succeeds)", () =>
    expect(Decode.(list(string, Sample.jsonArrayString)))
    |> toEqual(Some(Sample.valListString))
  );

  test("Decode list (succeeds on empty)", () =>
    expect(Decode.(list(string, Sample.jsonArrayEmpty)))
    |> toEqual(Some(Sample.valListEmpty))
  );

  test("Decode dict (succeeds)", () =>
    expect(Decode.(dict(floatFromNumber, Sample.jsonDictFloat)))
    |> toEqual(Some(Sample.valDictFloat))
  );

  test("Decode dict (succeeds on empty)", () =>
    expect(Decode.(dict(string, Sample.jsonDictEmpty)))
    |> toEqual(Some(Sample.valDictEmpty))
  );

  test("Decode dict (fails on record)", () =>
    expect(Decode.(dict(string, Sample.jsonPersonBill))) |> toEqual(None)
  );

  test("at (succeeds on nested field)", () =>
    expect(
      Decode.(
        at(["job", "manager", "job", "title"], string, Sample.jsonPersonBill)
      ),
    )
    |> toEqual(Some("CEO"))
  );

  test("at (succeeds on inner decode with no fields)", () =>
    expect(Decode.(at([], string, Sample.jsonString)))
    |> toEqual(Some(Sample.valString))
  );

  test("at (fails on missing field)", () =>
    expect(Decode.(at(["manager", "name"], string, Sample.jsonJobCeo)))
    |> toEqual(None)
  );

  test("at (fails on invalid inner data)", () =>
    expect(Decode.(at(["age"], string, Sample.jsonPersonBill)))
    |> toEqual(None)
  );

  test("field (succeeds)", () =>
    expect(Decode.(field("title", string, Sample.jsonJobCeo)))
    |> toEqual(Some("CEO"))
  );

  test("field (fails on missing)", () =>
    expect(Decode.(field("missing", string, Sample.jsonDictEmpty)))
    |> toEqual(None)
  );

  test("optionalField (succeeds)", () =>
    expect(Decode.(optionalField("name", string, Sample.jsonPersonBill)))
    |> toEqual(Some(Some("Bill")))
  );

  test("optionalField (succeeds on empty)", () =>
    expect(Decode.(optionalField("missing", boolean, Sample.jsonDictEmpty)))
    |> toEqual(Some(None))
  );

  test("optionalField (fails on non-object)", () =>
    expect(Decode.(optionalField("field", string, Sample.jsonString)))
    |> toEqual(None)
  );
});
