open Jest;
open Expect;
open Relude.Globals;

module Decode = Decode.AsResult.OfParseError;
module Sample = Decode_TestSampleData;

let valErr = (err, json) => Result.error(Decode.ParseError.Val(err, json));
let arrErr = (first, rest) =>
  Result.error(Decode.ParseError.Arr(NonEmpty.List.make(first, rest)));

let objErr = (first, rest) =>
  Result.error(Decode.ParseError.Obj(NonEmpty.List.make(first, rest)));

let objErrSingle = (field, err) => objErr((field, err), []);

describe("Simple decoders", () => {
  test("boolean", () =>
    expect(Decode.boolean(Sample.jsonNull))
    |> toEqual(valErr(`ExpectedBoolean, Sample.jsonNull))
  );

  test("string", () =>
    expect(Decode.string(Sample.jsonNull))
    |> toEqual(valErr(`ExpectedString, Sample.jsonNull))
  );

  test("floatFromNumber", () =>
    expect(Decode.floatFromNumber(Sample.jsonString))
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonString))
  );

  test("intFromNumber (non-number)", () =>
    expect(Decode.intFromNumber(Sample.jsonNull))
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonNull))
  );

  test("intFromNumber (float)", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat))
    |> toEqual(valErr(`ExpectedInt, Sample.jsonFloat))
  );

  test("date", () =>
    expect(Decode.date(Sample.jsonString))
    |> toEqual(valErr(`ExpectedValidDate, Sample.jsonString))
  );

  test("date (failure)", () =>
    expect(Decode.date(Sample.jsonNull))
    |> toEqual(
         Result.error(
           Decode.ParseError.(
             TriedMultiple(
               NonEmpty.List.make(
                 Val(`ExpectedNumber, Sample.jsonNull),
                 [Val(`ExpectedString, Sample.jsonNull)],
               ),
             )
           ),
         ),
       )
  );

  test("variant", () =>
    expect(Decode.variantFromString(Sample.colorFromJs, Sample.jsonString))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonString))
  );

  test("array", () =>
    expect(Decode.array(Decode.string, Sample.jsonNull))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("object", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonString))
    |> toEqual(valErr(`ExpectedObject, Sample.jsonString))
  );
});

describe("Inner decoders", () => {
  test("array", () =>
    expect(Decode.array(Decode.string, Sample.jsonArrayString))
    |> toEqual(Result.ok(Sample.valArrayString))
  );

  test("array (failure on inner decode)", () =>
    expect(Decode.array(Decode.boolean, Sample.jsonArrayString))
    |> toEqual(
         arrErr(
           (0, Val(`ExpectedBoolean, Js.Json.string("A"))),
           [
             (1, Val(`ExpectedBoolean, Js.Json.string("B"))),
             (2, Val(`ExpectedBoolean, Js.Json.string("C"))),
           ],
         ),
       )
  );

  test("tuple (fails on null)", () =>
    expect(Decode.(tuple2(string, boolean, Sample.jsonNull)))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("tuple (fails on wrong size)", () =>
    expect(Decode.(tuple2(string, boolean, Sample.jsonArrayEmpty)))
    |> toEqual(valErr(`ExpectedTuple(2), Sample.jsonArrayEmpty))
  );

  test("tuple (fails on inner decode)", () =>
    expect(Decode.(tuple2(boolean, string, Sample.jsonTuple)))
    |> toEqual(
         arrErr(
           (0, Val(`ExpectedBoolean, Js.Json.string("A"))),
           [(1, Val(`ExpectedString, Js.Json.boolean(true)))],
         ),
       )
  );

  test("field (missing)", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonJobCeo))
    |> toEqual(objErrSingle("x", MissingField))
  );

  test("field (failure on inner decode)", () =>
    expect(Decode.field("manager", Decode.string, Sample.jsonJobCeo))
    |> toEqual(
         objErrSingle(
           "manager",
           InvalidField(Val(`ExpectedString, Sample.jsonNull)),
         ),
       )
  );

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

  let failureString = {|Attempted multiple decoders, which all failed:
    Expected string but found {}
    Expected number but found {}
    Expected boolean but found {}|};

  test("oneOf (success on first)", () =>
    expect(decodeUnion(Sample.jsonString))
    |> toEqual(Result.ok(Sample.S(Sample.valString)))
  );

  test("oneOf (success on last)", () =>
    expect(decodeUnion(Sample.jsonBool))
    |> toEqual(Result.ok(Sample.B(Sample.valBool)))
  );

  test("oneOf (failure)", () =>
    expect(decodeUnion(Sample.jsonDictEmpty))
    |> toEqual(
         Result.error(
           Decode.ParseError.(
             TriedMultiple(
               NonEmpty.List.make(
                 Val(`ExpectedString, Sample.jsonDictEmpty),
                 [
                   Val(`ExpectedNumber, Sample.jsonDictEmpty),
                   Val(`ExpectedBoolean, Sample.jsonDictEmpty),
                 ],
               ),
             )
           ),
         ),
       )
  );

  test("oneOf (failure to string)", () =>
    expect(
      decodeUnion(Sample.jsonDictEmpty)
      |> Result.mapError(Decode.ParseError.failureToDebugString),
    )
    |> toEqual(Result.error(failureString))
  );
});

describe("Large, nested decoder", () => {
  let decodeJob =
    Decode.(
      map4(
        Sample.makeJob,
        field("title", string),
        field("x", string),
        field("title", date),
        pure(None),
      )
    );

  let decoded = decodeJob(Sample.jsonJobCeo);

  let error =
    Decode.ParseError.(
      Obj(
        NonEmpty.List.make(
          ("x", MissingField),
          [
            (
              "title",
              InvalidField(Val(`ExpectedValidDate, Js.Json.string("CEO"))),
            ),
          ],
        ),
      )
    );

  let objErrString = {|Failed to decode object:
    Field "x" is required, but was not present
    Field "title" had an invalid value: Expected a valid date but found "CEO"|};

  let arrErrString = {|Failed to decode array:
    At position 0: Expected string but found null|};

  test("map4, field", () =>
    expect(decoded) |> toEqual(Result.error(error))
  );

  test("toDebugString (obj)", () =>
    expect(Decode.ParseError.failureToDebugString(error))
    |> toEqual(objErrString)
  );

  test("toDebugString (arr)", () =>
    expect(
      Decode.ParseError.(
        arrPure(0, Val(`ExpectedString, Sample.jsonNull))
        |> failureToDebugString
      ),
    )
    |> toEqual(arrErrString)
  );
});

// ParseErrors only know how to combine Arr+Arr and Obj+Obj. In most situations
// this is all that matters. In all other cases, the first error is chosen.
describe("Parse error combinations", () => {
  let combine = (a, b) => (a, b);

  let arrError =
    Decode.ParseError.(
      Arr(
        NonEmpty.List.make(
          (0, Val(`ExpectedBoolean, Js.Json.string("A"))),
          [
            (1, Val(`ExpectedBoolean, Js.Json.string("B"))),
            (2, Val(`ExpectedBoolean, Js.Json.string("C"))),
          ],
        ),
      )
    );

  let objError = Decode.ParseError.(objPure("x", MissingField));

  test("combine Val/Val", () =>
    expect(Decode.(map2(combine, string, boolean, Sample.jsonNull)))
    |> toEqual(valErr(`ExpectedString, Sample.jsonNull))
  );

  test("combine Arr/Val", () =>
    expect(
      Decode.(map2(combine, list(boolean), boolean, Sample.jsonArrayString)),
    )
    |> toEqual(Result.error(arrError))
  );

  test("combine Obj/Val", () =>
    expect(
      Decode.(
        map2(combine, field("x", boolean), boolean, Sample.jsonDictEmpty)
      ),
    )
    |> toEqual(Result.error(objError))
  );
});
