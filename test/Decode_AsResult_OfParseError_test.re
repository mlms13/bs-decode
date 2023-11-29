open Jest;
open Expect;

module Sample = Decode_TestSampleData;
open Decode_AsResult_OfParseError;

let valErr = (err, json) => Error(ParseError.Val(err, json));
let arrErr = (first, rest) =>
  Error(ParseError.Arr(NonEmpty.List.make(first, rest)));

let objErr = (first, rest) =>
  Error(ParseError.Obj(NonEmpty.List.make(first, rest)));

let missingFieldErr = field =>
  Error(ParseError.(Obj(NonEmpty.List.make((field, MissingField), []))));

let invalidFieldErr = (field, err) =>
  Error(
    ParseError.(Obj(NonEmpty.List.make((field, InvalidField(err)), []))),
  );

let triedMultipleErr = (first, rest) =>
  Error(ParseError.TriedMultiple(NonEmpty.List.make(first, rest)));

describe("Basic deocders", () => {
  test("bool (success)", () =>
    Sample.jsonTrue |> boolean |> expect |> toEqual(Ok(Sample.valBool))
  );

  test("null (success)", () =>
    expect(null(Sample.jsonNull)) |> toEqual(Result.ok())
  );

  test("null (failure)", () =>
    expect(null(Sample.jsonFalse))
    |> toEqual(valErr(`ExpectedNull, Sample.jsonFalse))
  );

  test("bool (failure, null)", () =>
    Sample.jsonNull
    |> boolean
    |> expect
    |> toEqual(valErr(`ExpectedBoolean, Sample.jsonNull))
  );

  test("bool (failure, int)", () =>
    Sample.jsonIntOne
    |> boolean
    |> expect
    |> toEqual(valErr(`ExpectedBoolean, Sample.jsonIntOne))
  );

  test("bool (failure, string containing true)", () =>
    Sample.jsonStringTrue
    |> boolean
    |> expect
    |> toEqual(valErr(`ExpectedBoolean, Sample.jsonStringTrue))
  );

  test("string (success)", () =>
    Sample.jsonString |> string |> expect |> toEqual(Ok(Sample.valString))
  );

  test("string (success, string contains a number)", () =>
    Sample.jsonString4 |> string |> expect |> toEqual(Ok(Sample.valString4))
  );

  test("string (failure, null)", () =>
    Sample.jsonNull
    |> string
    |> expect
    |> toEqual(valErr(`ExpectedString, Sample.jsonNull))
  );

  test("string (failure, float)", () =>
    Sample.jsonFloat
    |> string
    |> expect
    |> toEqual(valErr(`ExpectedString, Sample.jsonFloat))
  );

  test("number (success)", () =>
    Sample.jsonFloat
    |> floatFromNumber
    |> expect
    |> toEqual(Ok(Sample.valFloat))
  );

  test("number (failure, null)", () =>
    Sample.jsonNull
    |> floatFromNumber
    |> expect
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonNull))
  );

  test("number (failure, string)", () =>
    Sample.jsonString
    |> floatFromNumber
    |> expect
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonString))
  );

  test("arrayJson (success)", () =>
    expect(arrayJson(Sample.jsonArrayString))
    |> toEqual(Result.ok([|"A", "B", "C"|] |> Array.map(Js.Json.string)))
  );

  test("arrayJson (failure, null)", () =>
    expect(arrayJson(Sample.jsonNull))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("listJson (success)", () =>
    expect(listJson(Sample.jsonArrayString))
    |> toEqual(Result.ok(["A", "B", "C"] |> List.map(Js.Json.string)))
  );

  test("listJson (failure, number)", () =>
    expect(arrayJson(Sample.jsonFloat))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonFloat))
  );

  test("dictJson (success)", () =>
    expect(dictJson(Sample.jsonDictEmpty))
    |> toEqual(Result.ok(Js.Dict.empty()))
  );

  test("dictJson (failure, array)", () =>
    expect(dictJson(Sample.jsonArrayEmpty))
    |> toEqual(valErr(`ExpectedObject, Sample.jsonArrayEmpty))
  );
});

describe("Validations", () => {
  test("int (success, 0)", () =>
    Sample.jsonIntZero |> intFromNumber |> expect |> toEqual(Ok(0))
  );

  test("int (success, 1)", () =>
    Sample.jsonIntOne |> intFromNumber |> expect |> toEqual(Ok(1))
  );

  test("int (failure, null)", () =>
    Sample.jsonNull
    |> intFromNumber
    |> expect
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonNull))
  );

  test("int (failure, string containing int)", () =>
    Sample.jsonString4
    |> intFromNumber
    |> expect
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonString4))
  );

  test("int (failure, float)", () =>
    Sample.jsonFloat
    |> intFromNumber
    |> expect
    |> toEqual(valErr(`ExpectedInt, Sample.jsonFloat))
  );

  test("int (failure, out of bounds)", () =>
    Sample.jsonLargeFloat
    |> intFromNumber
    |> expect
    |> toEqual(valErr(`ExpectedInt, Sample.jsonLargeFloat))
  );

  test("date (success, valid string)", () =>
    Sample.jsonDateString
    |> date
    |> expect
    |> toEqual(Ok(Sample.valDateString))
  );

  test("date (success, valid number)", () =>
    Sample.jsonDateNumber
    |> date
    |> expect
    |> toEqual(Ok(Sample.valDateNumber))
  );

  test("date (failure, null)", () =>
    Sample.jsonNull
    |> date
    |> expect
    |> toEqual(
         triedMultipleErr(
           Val(`ExpectedNumber, Sample.jsonNull),
           [Val(`ExpectedString, Sample.jsonNull)],
         ),
       )
  );

  test("date (failure, invalid string)", () =>
    Sample.jsonString
    |> date
    |> expect
    |> toEqual(valErr(`ExpectedValidDate, Sample.jsonString))
  );

  test("date (failure, invalid number)", () =>
    Js.Json.number(Js.Float._NaN)
    |> date
    |> expect
    |> toEqual(valErr(`ExpectedValidDate, Js.Json.number(Js.Float._NaN)))
  );

  test("literalString (success)", () =>
    Sample.jsonStringBlue
    |> literalString("blue")
    |> expect
    |> toEqual(Ok("blue"))
  );

  test("literalString (failure, wrong type)", () =>
    Sample.jsonIntFive
    |> literalString("blue")
    |> expect
    |> toEqual(valErr(`ExpectedString, Sample.jsonIntFive))
  );

  test("literalString (failure, wrong value)", () =>
    Sample.jsonStringYellow
    |> literalString("blue")
    |> expect
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonStringYellow))
  );

  test("literalInt (success)", () =>
    Sample.jsonIntFive |> literalInt(5) |> expect |> toEqual(Ok(5))
  );

  test("literalInt (failure, wrong type)", () =>
    Sample.jsonStringBlue
    |> literalInt(5)
    |> expect
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonStringBlue))
  );

  test("literalInt (failure, wrong value)", () =>
    Sample.jsonIntZero
    |> literalInt(5)
    |> expect
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonIntZero))
  );

  test("literalFloat (success)", () =>
    Sample.jsonIntFive |> literalFloat(5.0) |> expect |> toEqual(Ok(5.0))
  );

  test("literalFloat (failure, wrong type)", () =>
    Sample.jsonStringBlue
    |> literalFloat(5.0)
    |> expect
    |> toEqual(valErr(`ExpectedNumber, Sample.jsonStringBlue))
  );

  test("literalFloat (failure, wrong value)", () =>
    Sample.jsonIntZero
    |> literalFloat(5.0)
    |> expect
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonIntZero))
  );

  test("literalTrue (success)", () =>
    Sample.jsonTrue |> literalTrue |> expect |> toEqual(Ok(true))
  );

  test("literalTrue (failure, false)", () =>
    Sample.jsonFalse
    |> literalTrue
    |> expect
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonFalse))
  );

  test("literalFalse (success)", () =>
    Sample.jsonFalse |> literalFalse |> expect |> toEqual(Ok(false))
  );

  test("literalFalse (failure, true)", () =>
    Sample.jsonTrue
    |> literalFalse
    |> expect
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonTrue))
  );
});

describe("Arrays, lists, tuples", () => {
  test("array (success)", () =>
    Sample.jsonArrayString
    |> array(string)
    |> expect
    |> toEqual(Ok([|"A", "B", "C"|]))
  );

  test("array (success, empty)", () =>
    Sample.jsonArrayEmpty |> array(string) |> expect |> toEqual(Ok([||]))
  );

  test("array (success, nested)", () =>
    Sample.jsonArrayNested
    |> array(array(string))
    |> expect
    |> toEqual(Ok([|[|"a", "b"|], [||], [|"c"|]|]))
  );

  test("array (failure, null)", () =>
    Sample.jsonNull
    |> array(string)
    |> expect
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("array (failure, string)", () =>
    Sample.jsonString
    |> array(string)
    |> expect
    |> toEqual(valErr(`ExpectedArray, Sample.jsonString))
  );

  test("array (failure, inner decode)", () =>
    Sample.jsonArrayString
    |> array(boolean)
    |> expect
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
    expect(arrayAt(1, string, Sample.jsonArrayString))
    |> toEqual(Result.ok("B"))
  );

  test("arrayAt (failure, non-array)", () =>
    expect(arrayAt(3, string, Sample.jsonNull))
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("arrayAt (failure, empty array)", () =>
    expect(arrayAt(1, string, Sample.jsonArrayEmpty))
    |> toEqual(valErr(`ExpectedTuple(2), Sample.jsonArrayEmpty))
  );

  test("list (success)", () =>
    Sample.jsonArrayString
    |> list(string)
    |> expect
    |> toEqual(Ok(["A", "B", "C"]))
  );

  test("list (success, empty)", () =>
    Sample.jsonArrayEmpty |> list(string) |> expect |> toEqual(Ok([]))
  );
});

describe("Dictionaries, records", () => {
  test("dict (success, empty)", () =>
    Sample.jsonDictEmpty
    |> dict(okJson)
    |> expect
    |> toEqual(Ok(Js.Dict.empty()))
  );

  test("dict (success, non-empty with floats)", () =>
    Sample.jsonDictFloat
    |> dict(floatFromNumber)
    |> expect
    |> toEqual(Ok(Sample.valDictFloat))
  );

  test("dict (failure, null)", () =>
    Sample.jsonNull
    |> dict(okJson)
    |> expect
    |> toEqual(valErr(`ExpectedObject, Sample.jsonNull))
  );

  test("dict (failure, record)", () =>
    Sample.jsonPersonBill
    |> dict(string)
    |> expect
    |> toEqual(
         objErr(
           (
             "age",
             InvalidField(Val(`ExpectedString, Js.Json.number(27.0))),
           ),
           [
             ("job", InvalidField(Val(`ExpectedString, Sample.jsonJobBill))),
           ],
         ),
       )
  );

  test("at (succeeds on nested field)", () =>
    Sample.jsonPersonBill
    |> at(["job", "manager", "job", "title"], string)
    |> expect
    |> toEqual(Ok("CEO"))
  );

  test("at (succeeds on inner decode with no fields)", () =>
    Sample.jsonString
    |> at([], string)
    |> expect
    |> toEqual(Ok(Sample.valString))
  );

  test("at (fails on missing field)", () =>
    Sample.jsonJobCeo
    |> at(["manager", "name"], string)
    |> expect
    |> toEqual(
         invalidFieldErr("manager", Val(`ExpectedObject, Js.Json.null)),
       )
  );

  test("at (fails on invalid inner data)", () =>
    Sample.jsonPersonBill
    |> at(["age"], string)
    |> expect
    |> toEqual(
         invalidFieldErr("age", Val(`ExpectedString, Js.Json.number(27.0))),
       )
  );

  test("field (succeeds)", () =>
    Sample.jsonJobCeo
    |> field("title", string)
    |> expect
    |> toEqual(Ok("CEO"))
  );

  test("field (failure, non-object)", () =>
    Sample.jsonArrayEmpty
    |> field("x", string)
    |> expect
    |> toEqual(valErr(`ExpectedObject, Sample.jsonArrayEmpty))
  );

  test("field (failure, missing)", () =>
    Sample.jsonJobCeo
    |> field("x", string)
    |> expect
    |> toEqual(missingFieldErr("x"))
  );

  test("field (failure, inner decode)", () =>
    Sample.jsonJobCeo
    |> field("manager", string)
    |> expect
    |> toEqual(
         invalidFieldErr("manager", Val(`ExpectedString, Sample.jsonNull)),
       )
  );
});

describe("Alternatives, fallbacks, unions", () => {
  describe("One-of, alt", () => {
    test("alt (success, first when both are ok)", () =>
      Sample.jsonDateString
      |> alt(date |> map(a => `Date(a)), string |> map(a => `String(a)))
      |> expect
      |> toEqual(Ok(`Date(Sample.valDateString)))
    );

    test("alt (success, only first is ok)", () =>
      Sample.jsonString
      |> alt(string |> map(s => `S(s)), boolean |> map(b => `B(b)))
      |> expect
      |> toEqual(Ok(`S(Sample.valString)))
    );

    test("alt (success, only second is ok)", () =>
      Sample.jsonString
      |> alt(boolean |> map(b => `B(b)), string |> map(s => `S(s)))
      |> expect
      |> toEqual(Ok(`S(Sample.valString)))
    );

    test("alt (first success, second not evaluated)", () => {
      let decodeExplode = _json => failwith("Should not be evaluated");
      Sample.jsonTrue
      |> alt(boolean, decodeExplode)
      |> expect
      |> toEqual(Ok(true));
    });

    test("fallback (used for missing field)", () =>
      Sample.jsonDictEmpty
      |> fallback(field("x", boolean), false)
      |> expect
      |> toEqual(Ok(false))
    );

    test("fallback (used for invalid inner data)", () =>
      Sample.jsonJobCeo
      |> fallback(field("title", intFromNumber), 1)
      |> expect
      |> toEqual(Ok(1))
    );

    test("fallback (not used for successful decode)", () =>
      Sample.jsonJobCeo
      |> fallback(field("title", string), "default")
      |> expect
      |> toEqual(Ok("CEO"))
    );
  });

  describe("Optional values, fallbacks", () => {
    test("optional (success, float is present)", () =>
      Sample.jsonFloat
      |> optional(floatFromNumber)
      |> expect
      |> toEqual(Ok(Some(Sample.valFloat)))
    );

    test("optional (success, null)", () =>
      Sample.jsonNull
      |> optional(floatFromNumber)
      |> expect
      |> toEqual(Ok(None))
    );

    test("optional (failure, string)", () =>
      Sample.jsonString
      |> optional(floatFromNumber)
      |> expect
      |> toEqual(valErr(`ExpectedNumber, Sample.jsonString))
    );

    test("optionalField (success)", () =>
      Sample.jsonPersonBill
      |> optionalField("name", string)
      |> expect
      |> toEqual(Ok(Some("Bill")))
    );

    test("optionalField (success, missing field)", () =>
      Sample.jsonDictEmpty
      |> optionalField("x", boolean)
      |> expect
      |> toEqual(Ok(None))
    );

    test("optionalField (success, null value)", () =>
      Sample.jsonJobCeo
      |> optionalField("manager", boolean)
      |> expect
      |> toEqual(Ok(None))
    );

    test("optionalField (failure, non-object)", () =>
      Sample.jsonString
      |> optionalField("field", string)
      |> expect
      |> toEqual(valErr(`ExpectedObject, Sample.jsonString))
    );

    test("optionalField (failure, inner decode fails)", () =>
      Sample.jsonJobCeo
      |> optionalField("title", boolean)
      |> expect
      |> toEqual(
           invalidFieldErr(
             "title",
             Val(`ExpectedBoolean, Js.Json.string("CEO")),
           ),
         )
    );
  });

  describe("String union", () => {
    let decodePolyColor =
      stringUnion(("blue", `blue), [("red", `red), ("green", `green)]);

    test("stringUnion (success)", () =>
      Sample.jsonStringBlue
      |> decodePolyColor
      |> expect
      |> toEqual(Ok(`blue))
    );

    test("stringUnion (failure)", () =>
      Sample.jsonString4
      |> decodePolyColor
      |> expect
      |> toEqual(
           triedMultipleErr(
             Val(`ExpectedValidOption, Sample.jsonString4),
             [
               Val(`ExpectedValidOption, Sample.jsonString4),
               Val(`ExpectedValidOption, Sample.jsonString4),
             ],
           ),
         )
    );
  });

  describe("Int union", () => {
    let decodeIntColor = intUnion((0, `blue), [(1, `red), (2, `green)]);

    test("intUnion (success)", () =>
      Sample.jsonIntZero |> decodeIntColor |> expect |> toEqual(Ok(`blue))
    );

    test("intUnion (failure)", () =>
      Sample.jsonIntFive
      |> decodeIntColor
      |> expect
      |> toEqual(
           triedMultipleErr(
             Val(`ExpectedValidOption, Sample.jsonIntFive),
             [
               Val(`ExpectedValidOption, Sample.jsonIntFive),
               Val(`ExpectedValidOption, Sample.jsonIntFive),
             ],
           ),
         )
    );
  });

  describe("Custom union, oneOf", () => {
    let decodeUnion =
      oneOf(
        map(Sample.unionS, string),
        [
          map(Sample.unionN, optional(floatFromNumber)),
          map(Sample.unionB, boolean),
        ],
      );

    test("oneOf (success, first matches)", () =>
      Sample.jsonString
      |> decodeUnion
      |> expect
      |> toEqual(Ok(Sample.S(Sample.valString)))
    );

    test("oneOf (success, last matches)", () =>
      Sample.jsonTrue
      |> decodeUnion
      |> expect
      |> toEqual(Ok(Sample.B(true)))
    );

    test("oneOf (failure)", () =>
      Sample.jsonDictEmpty
      |> decodeUnion
      |> expect
      |> toEqual(
           triedMultipleErr(
             Val(`ExpectedString, Sample.jsonDictEmpty),
             [
               Val(`ExpectedNumber, Sample.jsonDictEmpty),
               Val(`ExpectedBoolean, Sample.jsonDictEmpty),
             ],
           ),
         )
    );
  });
});

describe("Decode utils", () => {
  test("map (success)", () =>
    Sample.jsonArrayString
    |> map(List.String.contains("A"), list(string))
    |> expect
    |> toEqual(Ok(true))
  );

  test("map (failure)", () =>
    Sample.jsonNull
    |> map(v => v == true, boolean)
    |> expect
    |> toEqual(valErr(`ExpectedBoolean, Sample.jsonNull))
  );

  test("apply", () =>
    Sample.jsonArrayString
    |> apply(_ => Ok(v => List.length(v) == 3), list(string))
    |> expect
    |> toEqual(Ok(true))
  );

  Skip.test("apply (collects multiple errors)", () =>
    expect(true) |> toEqual(true)
  );

  test("pair (success, both ok)", () =>
    Sample.jsonJobCeo
    |> pair(field("title", string), field("startDate", date))
    |> expect
    |> toEqual(Ok(("CEO", Js.Date.fromString("2016-04-01T00:00:00.0Z"))))
  );

  // test("pair (failure, first fails)")

  test("pure", () => {
    Sample.jsonNull |> pure(3) |> expect |> toEqual(Ok(3))
  });

  // BKMRK: this gets way cleaner when we add custom validation support
  let validateListHead = (a, json) =>
    a
    |> List.head
    |> Result.fromOption(ParseError.Val(`ExpectedValidOption, json));

  test("flatMap (success)", () =>
    Sample.jsonArrayString
    |> flatMap(validateListHead, list(string))
    |> expect
    |> toEqual(Ok("A"))
  );

  test("flatMap (failure, outer failure)", () =>
    Sample.jsonNull
    |> flatMap(validateListHead, list(string))
    |> expect
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("flatMap (failure, inner failure)", () =>
    Sample.jsonArrayEmpty
    |> flatMap(validateListHead, list(string))
    |> expect
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonArrayEmpty))
  );

  test("hush (success)", () => {
    let decodeBooleanOpt = boolean |> hush;
    expect(decodeBooleanOpt(Sample.jsonTrue)) |> toEqual(Some(true));
  });

  test("hush (failure)", () => {
    let decodeStringOpt = string |> hush;
    expect(decodeStringOpt(Sample.jsonNull)) |> toEqual(None);
  });
});

describe("Letops, infix", () => {
  test("letops map, apply (success)", () => {
    let decodeJob = {
      let+ title = field("title", string)
      and+ companyName = field("companyName", string)
      and+ startDate = field("startDate", date)
      and+ manager = pure(None);
      Sample.{title, companyName, startDate, manager};
    };

    Sample.jsonJobCeo |> decodeJob |> expect |> toEqual(Ok(Sample.jobCeo));
  });

  test("letops map, apply (failure, second field has wrong type)", () => {
    // companyName is intentionally wrong
    let decodeJob = {
      let+ title = field("title", string)
      and+ companyName = field("manager", string)
      and+ startDate = field("startDate", date)
      and+ manager = pure(None);
      Sample.{title, companyName, startDate, manager};
    };

    Sample.jsonJobCeo
    |> decodeJob
    |> expect
    |> toEqual(
         invalidFieldErr("manager", Val(`ExpectedString, Sample.jsonNull)),
       );
  });

  test("infix map, apply (success)", () => {
    let ((<$>), (<*>)) = (map, apply);
    let decodeJobInfix =
      Sample.makeJob
      <$> field("title", string)
      <*> field("companyName", string)
      <*> field("startDate", date)
      <*> pure(None);

    Sample.jsonJobCeo
    |> decodeJobInfix
    |> expect
    |> toEqual(Ok(Sample.jobCeo));
  });
});

describe("ParseError", () => {
  describe("Combinations", () => {
    // ParseErrors only know how to combine Arr+Arr and Obj+Obj. In most situations
    // this is all that matters. In all other cases, the first error is chosen.
    test("combine (Val/Val)", () =>
      Sample.jsonNull
      |> pair(string, boolean)
      |> expect
      |> toEqual(valErr(`ExpectedString, Sample.jsonNull))
    );

    test("combine (Arr/Val)", () =>
      Sample.jsonArrayString
      |> pair(list(boolean), boolean)
      |> expect
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

    test("combine (Obj/Val)", () =>
      Sample.jsonDictEmpty
      |> pair(field("x", boolean), boolean)
      |> expect
      |> toEqual(missingFieldErr("x"))
    );
  });

  describe("toDebugString", () => {
    let error =
      ParseError.(
        Obj(
          NonEmpty.List.make(
            ("x", MissingField),
            [
              (
                "title",
                InvalidField(
                  Val(`ExpectedValidDate, Js.Json.string("CEO")),
                ),
              ),
            ],
          ),
        )
      );

    let objErrString = {|Failed to decode object:
    Field "x" is required, but was not present
    Field "title" had an invalid value: Expected a valid date but found "CEO"|};

    test("toDebugString (obj)", () =>
      error
      |> ParseError.failureToDebugString
      |> expect
      |> toEqual(objErrString)
    );

    test("toDebugString (arr)", () =>
      ParseError.arrPure(0, Val(`ExpectedString, Sample.jsonNull))
      |> ParseError.failureToDebugString
      |> expect
      |> toEqual(
           {|Failed to decode array:
    At position 0: Expected string but found null|},
         )
    );

    test("oneOf (failure to string)", () => {
      let failureString = {|Attempted multiple decoders, which all failed:
    Expected string but found {}
    Expected number but found {}
    Expected boolean but found {}|};

      Sample.jsonDictEmpty
      |> oneOf(
           map(Sample.unionS, string),
           [
             map(Sample.unionN, optional(floatFromNumber)),
             map(Sample.unionB, boolean),
           ],
         )
      |> Result.mapError(ParseError.failureToDebugString)
      |> expect
      |> toEqual(Error(failureString));
    });
  });
});

[@ocaml.warning "-3"]
describe("Deprecated decoders", () => {
  test("variant", () =>
    expect(variantFromString(Sample.colorFromJs, Sample.jsonString))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonString))
  );

  test("variantFromString (success)", () =>
    expect(variantFromString(Sample.colorFromJs, Sample.jsonStringBlue))
    |> toEqual(Ok(`blue))
  );

  test("variantFromString (failure)", () =>
    expect(variantFromString(Sample.colorFromJs, Sample.jsonString))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonString))
  );

  test("variantFromInt (success)", () =>
    expect(variantFromInt(Sample.numbersFromJs, Sample.jsonIntZero))
    |> toEqual(Ok(Sample.Zero))
  );

  test("variantFromInt (failure)", () =>
    expect(variantFromInt(Sample.numbersFromJs, Sample.jsonIntFive))
    |> toEqual(valErr(`ExpectedValidOption, Sample.jsonIntFive))
  );

  let floatMapEq = (a, b) => Belt.Map.String.eq(a, b, Float.eq);

  test("stringMap (success)", () =>
    Sample.jsonDictFloat
    |> (stringMap(floatFromNumber) |> map(floatMapEq(Sample.valMapFloat)))
    |> expect
    |> toEqual(Ok(true))
  );

  test("stringMap (failure, null)", () =>
    Sample.jsonNull
    |> (stringMap(floatFromNumber) |> map(floatMapEq(Sample.valMapFloat)))
    |> expect
    |> toEqual(valErr(`ExpectedObject, Js.Json.null))
  );

  test("tuple (success)", () =>
    Sample.jsonTuple
    |> tuple(string, boolean)
    |> expect
    |> toEqual(Ok(("A", true)))
  );

  test("tuple (failure, non-array)", () =>
    Sample.jsonNull
    |> tuple2(string, boolean)
    |> expect
    |> toEqual(valErr(`ExpectedArray, Sample.jsonNull))
  );

  test("tuple (failure, wrong size)", () =>
    Sample.jsonArrayEmpty
    |> tuple2(string, boolean)
    |> expect
    |> toEqual(valErr(`ExpectedTuple(2), Sample.jsonArrayEmpty))
  );

  test("tuple (failure, inner decode)", () =>
    Sample.jsonTuple
    |> tuple2(boolean, string)
    |> expect
    |> toEqual(
         arrErr(
           (0, Val(`ExpectedBoolean, Js.Json.string("A"))),
           [(1, Val(`ExpectedString, Js.Json.boolean(true)))],
         ),
       )
  );

  test("tuple3 (success)", () =>
    Sample.jsonTuple3
    |> tuple3(string, boolean, intFromNumber)
    |> expect
    |> toEqual(Ok(("A", true, 3)))
  );

  test("tuple4 (success)", () =>
    Sample.jsonTuple4
    |> tuple4(string, boolean, boolean, string)
    |> expect
    |> toEqual(Ok(("A", true, false, "B")))
  );

  test("tuple5 (success)", () =>
    Sample.jsonTuple5
    |> tuple5(string, string, string, string, string)
    |> expect
    |> toEqual(Ok(("A", "B", "C", "D", "E")))
  );

  test("tupleAtLeast2", () =>
    Sample.jsonTuple6
    |> tupleAtLeast2(string, string)
    |> expect
    |> toEqual(Ok(("A", "B")))
  );

  test("tupleAtLeast3", () =>
    Sample.jsonTuple6
    |> tupleAtLeast3(string, string, string)
    |> expect
    |> toEqual(Ok(("A", "B", "C")))
  );

  test("tupleAtLeast4", () =>
    Sample.jsonTuple6
    |> tupleAtLeast4(string, string, string, string)
    |> expect
    |> toEqual(Ok(("A", "B", "C", "D")))
  );

  test("tupleAtLeast5", () =>
    Sample.jsonTuple6
    |> tupleAtLeast5(string, string, string, string, string)
    |> expect
    |> toEqual(Ok(("A", "B", "C", "D", "E")))
  );

  test("tupleFromFields (success)", () =>
    Sample.jsonPersonBill
    |> tupleFromFields(("name", string), ("age", intFromNumber))
    |> expect
    |> toEqual(Ok(("Bill", 27)))
  );

  test("tupleFromFields (failure)", () =>
    tupleFromFields(("x", string), ("y", string), Sample.jsonDictEmpty)
    |> expect
    |> toEqual(objErr(("x", MissingField), [("y", MissingField)]))
  );
});

// here we import a gigantic json file (as raw json, to avoid slowing down the
// compiler)
[@mel.module] external bigjson: Js.Json.t = "./utils/BigJson.json";

describe("Big JSON", () => {
  test("is stack-safe", () =>
    bigjson |> array(okJson) |> Result.isOk |> expect |> toEqual(true)
  )
});
