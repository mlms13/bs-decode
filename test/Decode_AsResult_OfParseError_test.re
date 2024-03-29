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

describe("Decode utils", () => {
  test("hush (success)", () => {
    let decodeBooleanOpt = Decode.(boolean |> hush);
    expect(decodeBooleanOpt(Sample.jsonTrue)) |> toEqual(Some(true));
  });

  test("hush (failure)", () => {
    let decodeStringOpt = Decode.(string |> hush);
    expect(decodeStringOpt(Sample.jsonNull)) |> toEqual(None);
  });
});

describe("Simple decoders", () => {
  let decodeIntColor =
    Decode.intUnion((0, `blue), [(1, `red), (2, `green)]);

  test("null (success)", () =>
    expect(Decode.null(Sample.jsonNull)) |> toEqual(Result.ok())
  );

  test("null (failure)", () =>
    expect(Decode.null(Sample.jsonFalse))
    |> toEqual(valErr(`ExpectedNull, Sample.jsonFalse))
  );

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

  test("intFromNumber (string containing int)", () =>
    expect(Decode.intFromNumber(Sample.jsonString4))
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonString4))
  );

  test("intFromNumber (out-of-range int)", () =>
    expect(Decode.intFromNumber(Sample.jsonLargeFloat))
    |> toEqual(valErr(`ExpectedInt, Sample.jsonLargeFloat))
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

  test("literalTrue (success)", () =>
    expect(Decode.literalTrue(Sample.jsonTrue)) |> toEqual(Result.ok(true))
  );

  test("literalTrue (failure)", () =>
    expect(Decode.literalTrue(Sample.jsonFalse))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonFalse))
  );

  test("literalFalse (success)", () =>
    expect(Decode.literalFalse(Sample.jsonFalse))
    |> toEqual(Result.ok(false))
  );

  test("literalFalse (failure)", () =>
    expect(Decode.literalFalse(Sample.jsonTrue))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonTrue))
  );

  test("intUnion (success)", () =>
    expect(decodeIntColor(Sample.jsonIntZero)) |> toEqual(Result.ok(`blue))
  );

  test("intUnion (failure)", () =>
    expect(decodeIntColor(Sample.jsonIntFive))
    |> toEqual(
         Result.error(
           Decode.ParseError.(
             TriedMultiple(
               NonEmpty.List.make(
                 Val(`ExpectedValidOption, Sample.jsonIntFive),
                 [
                   Val(`ExpectedValidOption, Sample.jsonIntFive),
                   Val(`ExpectedValidOption, Sample.jsonIntFive),
                 ],
               ),
             )
           ),
         ),
       )
  );

  test("variant", () =>
    [@ocaml.warning "-3"]
    expect(Decode.variantFromString(Sample.colorFromJs, Sample.jsonString))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonString))
  );

  test("array", () =>
    expect(Decode.array(Decode.string, Sample.jsonNull))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("arrayJson (success)", () =>
    expect(Decode.arrayJson(Sample.jsonArrayString))
    |> toEqual(Result.ok([|"A", "B", "C"|] |> Array.map(Js.Json.string)))
  );

  test("arrayJson (failure, null)", () =>
    expect(Decode.arrayJson(Sample.jsonNull))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("listJson (success)", () =>
    expect(Decode.listJson(Sample.jsonArrayString))
    |> toEqual(Result.ok(["A", "B", "C"] |> List.map(Js.Json.string)))
  );

  test("listJson (failure, number)", () =>
    expect(Decode.arrayJson(Sample.jsonFloat))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonFloat))
  );

  test("object", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonString))
    |> toEqual(valErr(`ExpectedObject, Sample.jsonString))
  );

  test("dictJson (success)", () =>
    expect(Decode.dictJson(Sample.jsonDictEmpty))
    |> toEqual(Result.ok(Js.Dict.empty()))
  );

  test("dictJson (failure, array)", () =>
    expect(Decode.dictJson(Sample.jsonArrayEmpty))
    |> toEqual(valErr(`ExpectedObject, Sample.jsonArrayEmpty))
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

  test("arrayAt (success)", () =>
    expect(Decode.arrayAt(1, Decode.string, Sample.jsonArrayString))
    |> toEqual(Result.ok("B"))
  );

  test("arrayAt (failure, non-array)", () =>
    expect(Decode.arrayAt(3, Decode.string, Sample.jsonNull))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("arrayAt (failure, empty array)", () =>
    expect(Decode.arrayAt(1, Decode.string, Sample.jsonArrayEmpty))
    |> toEqual(valErr(`ExpectedTuple(2), Sample.jsonArrayEmpty))
  );

  test("tuple (fails on null)", () =>
    [@ocaml.warning "-3"]
    expect(Decode.(tuple2(string, boolean, Sample.jsonNull)))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("tuple (fails on wrong size)", () =>
    [@ocaml.warning "-3"]
    expect(Decode.(tuple2(string, boolean, Sample.jsonArrayEmpty)))
    |> toEqual(valErr(`ExpectedTuple(2), Sample.jsonArrayEmpty))
  );

  test("tuple (fails on inner decode)", () =>
    [@ocaml.warning "-3"]
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

  test("optionalField (failure on outer structure)", () =>
    expect(Decode.optionalField("field", Decode.string, Sample.jsonString))
    |> toEqual(valErr(`ExpectedObject, Sample.jsonString))
  );

  test("optionalField (failure on inner decode)", () =>
    expect(Decode.(optionalField("title", boolean, Sample.jsonJobCeo)))
    |> toEqual(
         objErrSingle(
           "title",
           InvalidField(Val(`ExpectedBoolean, Js.Json.string("CEO"))),
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
    expect(decodeUnion(Sample.jsonTrue))
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
  [@ocaml.warning "-3"]
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
    [@ocaml.warning "-3"]
    expect(Decode.(map2(combine, string, boolean, Sample.jsonNull)))
    |> toEqual(valErr(`ExpectedString, Sample.jsonNull))
  );

  test("combine Arr/Val", () =>
    [@ocaml.warning "-3"]
    expect(
      Decode.(map2(combine, list(boolean), boolean, Sample.jsonArrayString)),
    )
    |> toEqual(Result.error(arrError))
  );

  test("combine Obj/Val", () =>
    [@ocaml.warning "-3"]
    expect(
      Decode.(
        map2(combine, field("x", boolean), boolean, Sample.jsonDictEmpty)
      ),
    )
    |> toEqual(Result.error(objError))
  );
});
