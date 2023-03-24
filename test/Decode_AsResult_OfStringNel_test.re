open Jest;
open Expect;
open Relude.Globals;

[@ocaml.warning "-3"]
module Decode = Decode.AsResult.OfStringNel;
module Sample = Decode_TestSampleData;
module Nel = Decode.NonEmptyList;

let makeErr = (msg, json) =>
  Result.error(Nel.pure(msg ++ " " ++ Js.Json.stringify(json)));

describe("Simple decode errors", () => {
  test("boolean", () =>
    expect(Decode.boolean(Sample.jsonNull))
    |> toEqual(makeErr("Expected boolean but found", Sample.jsonNull))
  );

  test("string", () =>
    expect(Decode.string(Sample.jsonNull))
    |> toEqual(makeErr("Expected string but found", Sample.jsonNull))
  );

  test("floatFromNumber", () =>
    expect(Decode.floatFromNumber(Sample.jsonNull))
    |> toEqual(makeErr("Expected number but found", Sample.jsonNull))
  );

  test("intFromNumber (non-number)", () =>
    expect(Decode.intFromNumber(Sample.jsonNull))
    |> toEqual(makeErr("Expected number but found", Sample.jsonNull))
  );

  test("intFromNumber (float)", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat))
    |> toEqual(makeErr("Expected int but found", Sample.jsonFloat))
  );

  test("date", () =>
    expect(Decode.date(Sample.jsonString))
    |> toEqual(makeErr("Expected a valid date but found", Sample.jsonString))
  );

  test("variant", () =>
    expect(Decode.variantFromString(Sample.colorFromJs, Sample.jsonString))
    |> toEqual(
         makeErr("Expected a valid option but found", Sample.jsonString),
       )
  );

  test("array", () =>
    expect(Decode.array(Decode.string, Sample.jsonNull))
    |> toEqual(makeErr("Expected array but found", Sample.jsonNull))
  );

  test("tuple", () =>
    expect(Decode.(tuple(string, boolean, Sample.jsonArrayEmpty)))
    |> toEqual(
         makeErr("Expected tuple of size 2 but found", Sample.jsonArrayEmpty),
       )
  );

  test("object", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonArrayEmpty))
    |> toEqual(makeErr("Expected object but found", Sample.jsonArrayEmpty))
  );
});

describe("Inner decoders", () => {
  test("array inner value (success)", () =>
    expect(Decode.(array(string, Sample.jsonArrayString)))
    |> toEqual(Result.ok(Sample.valArrayString))
  );

  test("array inner value (failure)", () =>
    expect(Decode.(array(boolean, Sample.jsonArrayString)))
    |> toEqual(
         Result.error(
           NonEmpty.List.make(
             "While decoding array, at position 0: Expected boolean but found \"A\"",
             [
               "While decoding array, at position 1: Expected boolean but found \"B\"",
               "While decoding array, at position 2: Expected boolean but found \"C\"",
             ],
           ),
         ),
       )
  );

  test("field (missing)", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonDictEmpty))
    |> toEqual(
         Result.error(NonEmpty.List.pure("Object field \"x\" was missing")),
       )
  );

  test("field (multiple, failure on second)", () =>
    expect(
      Decode.(
        map4(
          Sample.makeJob,
          field("title", string),
          field("manager", string),
          field("startDate", date),
          pure(None),
          Sample.jsonJobCeo,
        )
      ),
    )
    |> toEqual(
         makeErr(
           "While decoding object, for field \"manager\": Expected string but found",
           Js.Json.null,
         ),
       )
  );

  test("oneOf", () => {
    let decodeUnion =
      Decode.(
        oneOf(
          map(Sample.unionS, string),
          [
            map(Sample.unionN, optional(floatFromNumber)),
            map(Sample.unionB, boolean),
          ],
        )
      );

    expect(decodeUnion(Sample.jsonTrue))
    |> toEqual(Result.ok(Sample.(B(valBool))));
  });
});
