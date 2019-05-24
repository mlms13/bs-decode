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

  test("intFromNumber (non-number)", () =>
    expect(Decode.intFromNumber(Sample.jsonNull))
    |> toEqual(Result.error(Err.Val(`ExpectedNumber, Sample.jsonNull)))
  );

  test("intFromNumber (float)", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat))
    |> toEqual(Result.error(Err.Val(`ExpectedInt, Sample.jsonFloat)))
  );

  test("date", () =>
    expect(Decode.date(Sample.jsonString))
    |> toEqual(Result.error(Err.Val(`ExpectedValidDate, Sample.jsonString)))
  );

  test("variant", () =>
    expect(Decode.variantFromString(Sample.colorFromJs, Sample.jsonString))
    |> toEqual(
         Result.error(Err.Val(`ExpectedValidOption, Sample.jsonString)),
       )
  );

  test("array", () =>
    expect(Decode.array(Decode.string, Sample.jsonNull))
    |> toEqual(Result.error(Err.Val(`ExpectedArray, Sample.jsonNull)))
  );

  test("object", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonString))
    |> toEqual(Result.error(Err.Val(`ExpectedObject, Sample.jsonString)))
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
         Result.error(
           Err.Arr(
             NonEmpty.List.make(
               (0, Err.Val(`ExpectedBoolean, Js.Json.string("A"))),
               [
                 (1, Err.Val(`ExpectedBoolean, Js.Json.string("B"))),
                 (2, Err.Val(`ExpectedBoolean, Js.Json.string("C"))),
               ],
             ),
           ),
         ),
       )
  );

  test("field (missing)", () =>
    expect(Decode.field("x", Decode.string, Sample.jsonJobCeo))
    |> toEqual(
         Result.error(Err.Obj(NonEmpty.List.pure(("x", Err.MissingField)))),
       )
  );

  test("field (failure on inner decode)", () =>
    expect(Decode.field("manager", Decode.string, Sample.jsonJobCeo))
    |> toEqual(
         Result.error(
           Err.Obj(
             NonEmpty.List.pure((
               "manager",
               Err.InvalidField(Err.Val(`ExpectedString, Sample.jsonNull)),
             )),
           ),
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

    expect(decodeUnion(Sample.jsonBool))
    |> toEqual(Result.ok(Sample.(B(valBool))));
  });
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
    Err.Obj(
      NonEmpty.List.make(
        ("x", Err.MissingField),
        [
          (
            "title",
            Err.InvalidField(
              Err.Val(`ExpectedValidDate, Js.Json.string("CEO")),
            ),
          ),
        ],
      ),
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
    expect(Err.failureToDebugString(error)) |> toEqual(objErrString)
  );

  test("toDebugString (arr)", () =>
    expect(
      Err.(
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
    Err.Arr(
      NonEmpty.List.make(
        (0, Err.Val(`ExpectedBoolean, Js.Json.string("A"))),
        [
          (1, Err.Val(`ExpectedBoolean, Js.Json.string("B"))),
          (2, Err.Val(`ExpectedBoolean, Js.Json.string("C"))),
        ],
      ),
    );

  let objError = Err.objPure("x", Err.MissingField);

  test("combine Val/Val", () =>
    expect(Decode.(map2(combine, string, boolean, Sample.jsonNull)))
    |> toEqual(Result.error(Err.Val(`ExpectedString, Sample.jsonNull)))
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
