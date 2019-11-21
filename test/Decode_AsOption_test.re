open Jest;
open Expect;
open Relude.Globals;

module Decode = Decode.AsOption;
module Sample = Decode_TestSampleData;

describe("Simple decoders", () => {
  test("bool (success)", () =>
    expect(Decode.boolean(Sample.jsonBool))
    |> toEqual(Some(Sample.valBool))
  );

  test("bool (fails on null)", () =>
    expect(Decode.boolean(Sample.jsonNull)) |> toEqual(None)
  );

  test("bool (fails on int)", () =>
    expect(Decode.boolean(Sample.jsonInt)) |> toEqual(None)
  );

  test("bool (fails on string)", () =>
    expect(Decode.boolean(Sample.jsonStringTrue)) |> toEqual(None)
  );

  test("string (success)", () =>
    expect(Decode.string(Sample.jsonString))
    |> toEqual(Some(Sample.valString))
  );

  test("string (is still string when it contains a number)", () =>
    expect(Decode.string(Sample.jsonString4))
    |> toEqual(Some(Sample.valString4))
  );

  test("string (fails on float)", () =>
    expect(Decode.string(Sample.jsonFloat)) |> toEqual(None)
  );

  test("float (success)", () =>
    expect(Decode.floatFromNumber(Sample.jsonFloat))
    |> toEqual(Some(Sample.valFloat))
  );

  test("float (fails on string)", () =>
    expect(Decode.floatFromNumber(Sample.jsonString)) |> toEqual(None)
  );

  test("int (success)", () =>
    expect(Decode.intFromNumber(Sample.jsonInt))
    |> toEqual(Some(Sample.valInt))
  );

  test("int (succeeds on zero)", () =>
    expect(Decode.intFromNumber(Sample.jsonIntZero))
    |> toEqual(Some(Sample.valIntZero))
  );

  test("int (fails on string)", () =>
    expect(Decode.intFromNumber(Sample.jsonString)) |> toEqual(None)
  );

  test("int (fails on float)", () =>
    expect(Decode.intFromNumber(Sample.jsonFloat)) |> toEqual(None)
  );

  test("date (succeeds on valid string)", () =>
    expect(Decode.date(Sample.jsonDateString))
    |> toEqual(Some(Sample.valDateString))
  );

  test("date (succeeds on valid number)", () =>
    expect(Decode.date(Sample.jsonDateNumber))
    |> toEqual(Some(Sample.valDateNumber))
  );

  test("date (fails on invalid string)", () =>
    expect(Decode.date(Sample.jsonString)) |> toEqual(None)
  );

  test("date (fails on invalid number)", () =>
    expect(Decode.date(Js.Json.number(Js.Float._NaN))) |> toEqual(None)
  );

  test("date (fails on null)", () =>
    expect(Decode.date(Sample.jsonNull)) |> toEqual(None)
  );
});

describe("Variant decoders", () => {
  test("variantFromString (success)", () =>
    expect(
      Decode.variantFromString(Sample.colorFromJs, Sample.jsonStringBlue),
    )
    |> toEqual(Some(`blue))
  );

  test("variantFromString (failure)", () =>
    expect(
      Decode.variantFromString(Sample.colorFromJs, Sample.jsonStringYellow),
    )
    |> toEqual(None)
  );

  test("variantFromInt (success)", () =>
    expect(Decode.variantFromInt(Sample.numbersFromJs, Sample.jsonIntZero))
    |> toEqual(Some(Sample.Zero))
  );

  test("variantFromInt (failure)", () =>
    expect(Decode.variantFromInt(Sample.numbersFromJs, Sample.jsonIntFive))
    |> toEqual(None)
  );
});

describe("Nested decoders", () => {
  test("optional float (succeeds on null)", () =>
    expect(Decode.(optional(floatFromNumber, Sample.jsonNull)))
    |> toEqual(Some(None))
  );

  test("optional float (succeeds on float)", () =>
    expect(Decode.(optional(floatFromNumber, Sample.jsonFloat)))
    |> toEqual(Some(Some(Sample.valFloat)))
  );

  test("optional float (fails on bool)", () =>
    expect(Decode.(optional(floatFromNumber, Sample.jsonBool)))
    |> toEqual(None)
  );

  test("array (succeeds)", () =>
    expect(Decode.(array(string, Sample.jsonArrayString)))
    |> toEqual(Some(Sample.valArrayString))
  );

  test("array (succeeds on empty)", () =>
    expect(Decode.(array(string, Sample.jsonArrayEmpty)))
    |> toEqual(Some(Sample.valArrayEmpty))
  );

  test("array (succeeds on nested)", () =>
    expect(Decode.(array(array(string), Sample.jsonArrayNested)))
    |> toEqual(Some(Sample.valArrayNested))
  );

  test("array (fails on non-array)", () =>
    expect(Decode.(array(string, Sample.jsonString))) |> toEqual(None)
  );

  test("array (fails on invalid inner data)", () =>
    expect(Decode.(array(boolean, Sample.jsonArrayString))) |> toEqual(None)
  );

  test("list (succeeds)", () =>
    expect(Decode.(list(string, Sample.jsonArrayString)))
    |> toEqual(Some(Sample.valListString))
  );

  test("list (succeeds on empty)", () =>
    expect(Decode.(list(string, Sample.jsonArrayEmpty)))
    |> toEqual(Some(Sample.valListEmpty))
  );

  test("tuple2 (succeeds)", () =>
    expect(Decode.(tuple(string, boolean, Sample.jsonTuple)))
    |> toEqual(Some(Sample.valTuple))
  );

  test("tuple2 (fails on non-array)", () =>
    expect(Decode.(tuple(string, boolean, Sample.jsonBool)))
    |> toEqual(None)
  );

  test("tuple2 (fails on wrong size)", () =>
    expect(Decode.(tuple(string, boolean, Sample.jsonArrayEmpty)))
    |> toEqual(None)
  );

  test("tuple2 (fails on bad inner value)", () =>
    expect(Decode.(tuple(string, string, Sample.jsonTuple)))
    |> toEqual(None)
  );

  test("tuple3 (succeeds)", () =>
    expect(
      Decode.(tuple3(string, boolean, intFromNumber, Sample.jsonTuple3)),
    )
    |> toEqual(Some(Sample.valTuple3))
  );

  test("tuple4 (succeeds)", () =>
    expect(
      Decode.(tuple4(string, boolean, boolean, string, Sample.jsonTuple4)),
    )
    |> toEqual(Some(Sample.valTuple4))
  );

  test("tuple5 (succeeds)", () =>
    expect(
      Decode.(
        tuple5(string, string, string, string, string, Sample.jsonTuple5)
      ),
    )
    |> toEqual(Some(Sample.valTuple5))
  );

  test("tupleAtLeast2", () =>
    expect(Decode.(tupleAtLeast2(string, string, Sample.jsonTuple6)))
    |> toEqual(Some(("A", "B")))
  );

  test("tupleAtLeast3", () =>
    expect(Decode.(tupleAtLeast3(string, string, string, Sample.jsonTuple6)))
    |> toEqual(Some(("A", "B", "C")))
  );

  test("tupleAtLeast4", () =>
    expect(
      Decode.(
        tupleAtLeast4(string, string, string, string, Sample.jsonTuple6)
      ),
    )
    |> toEqual(Some(("A", "B", "C", "D")))
  );

  test("tupleAtLeast5", () =>
    expect(
      Decode.(
        tupleAtLeast5(
          string,
          string,
          string,
          string,
          string,
          Sample.jsonTuple6,
        )
      ),
    )
    |> toEqual(Some(("A", "B", "C", "D", "E")))
  );

  test("dict (succeeds)", () =>
    expect(Decode.(dict(floatFromNumber, Sample.jsonDictFloat)))
    |> toEqual(Some(Sample.valDictFloat))
  );

  test("dict (succeeds on empty)", () =>
    expect(Decode.(dict(string, Sample.jsonDictEmpty)))
    |> toEqual(Some(Sample.valDictEmpty))
  );

  test("dict (fails on record)", () =>
    expect(Decode.(dict(string, Sample.jsonPersonBill))) |> toEqual(None)
  );

  test("stringMap (succeeds)", () =>
    expect(
      Decode.map(
        Belt.Map.String.eq(Sample.valMapFloat, _, Float.eq),
        Decode.(stringMap(floatFromNumber)),
        Sample.jsonDictFloat,
      ),
    )
    |> toEqual(Some(true))
  );

  test("stringMap (fails)", () =>
    expect(
      Decode.map(
        Belt.Map.String.eq(Sample.valMapFloat, _, Float.eq),
        Decode.(stringMap(floatFromNumber)),
        Sample.jsonNull,
      ),
    )
    |> toEqual(None)
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
    expect(Decode.(field("x", string, Sample.jsonDictEmpty)))
    |> toEqual(None)
  );

  test("optionalField (succeeds)", () =>
    expect(Decode.(optionalField("name", string, Sample.jsonPersonBill)))
    |> toEqual(Some(Some("Bill")))
  );

  test("optionalField (succeeds on empty)", () =>
    expect(Decode.(optionalField("x", boolean, Sample.jsonDictEmpty)))
    |> toEqual(Some(None))
  );

  test("optionalField (fails on non-object)", () =>
    expect(Decode.(optionalField("field", string, Sample.jsonString)))
    |> toEqual(None)
  );

  test("tupleFromFields (success)", () =>
    expect(
      Decode.(
        tupleFromFields(
          ("name", string),
          ("age", intFromNumber),
          Sample.jsonPersonBill,
        )
      ),
    )
    |> toEqual(Some(("Bill", 27)))
  );

  test("tupleFromFields (failure)", () =>
    expect(
      Decode.(
        tupleFromFields(("x", string), ("y", string), Sample.jsonDictEmpty)
      ),
    )
    |> toEqual(None)
  );
});

describe("Decode with alternatives/fallbacks", () => {
  let decodeExplode = _json => failwith("Explosion");
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

  test("alt (doesn't evaluate second if first succeeds)", () =>
    expect(Decode.(alt(string, decodeExplode, Sample.jsonString)))
    |> toEqual(Some(Sample.valString))
  );

  test("oneOf (succeeds on first)", () =>
    expect(decodeUnion(Sample.jsonString))
    |> toEqual(Some(Sample.(S(valString))))
  );

  test("oneOf (succeeds on last)", () =>
    expect(decodeUnion(Sample.jsonBool))
    |> toEqual(Some(Sample.(B(valBool))))
  );

  test("oneOf (failure)", () =>
    expect(decodeUnion(Sample.jsonPersonBill)) |> toEqual(None)
  );

  test("fallback (used on missing field)", () =>
    expect(
      Decode.(fallback(field("x", boolean), false, Sample.jsonDictEmpty)),
    )
    |> toEqual(Some(false))
  );

  test("fallback (used on invalid inner data)", () =>
    expect(
      Decode.(fallback(field("title", intFromNumber), 1, Sample.jsonJobCeo)),
    )
    |> toEqual(Some(1))
  );
});

describe("Decode map, apply, pure, flatMap, etc", () => {
  test("map (success)", () =>
    expect(
      Decode.(
        map(List.String.contains("A"), list(string), Sample.jsonArrayString)
      ),
    )
    |> toEqual(Some(true))
  );

  test("map (failure)", () =>
    expect(Decode.(map(v => v == true, boolean, Sample.jsonNull)))
    |> toEqual(None)
  );

  test("apply", () =>
    expect(
      Decode.apply(
        _ => Some(v => List.length(v) == 3),
        Decode.(list(string)),
        Sample.jsonArrayString,
      ),
    )
    |> toEqual(Some(true))
  );

  test("pure", () => {
    let decode = Decode.pure(3);
    expect(decode(Sample.jsonNull)) |> toEqual(Some(3));
  });

  test("flatMap (success)", () =>
    expect(
      Decode.(
        Sample.jsonArrayString |> flatMap(List.head >> const, list(string))
      ),
    )
    |> toEqual(Some("A"))
  );

  test("flatMap (failure on inner failure)", () =>
    expect(
      Decode.(
        Sample.jsonArrayEmpty |> flatMap(List.head >> const, list(string))
      ),
    )
    |> toEqual(None)
  );

  test("flatMap (failure on outer failure)", () =>
    expect(
      Decode.(Sample.jsonNull |> flatMap(List.head >> const, list(string))),
    )
    |> toEqual(None)
  );
});

describe("Decode records", () => {
  let noManager = (title, company, start) =>
    Sample.makeJob(title, company, start, None);

  let decodeJobMap3 =
    Decode.(
      map3(
        noManager,
        field("title", string),
        field("companyName", string),
        field("startDate", date),
      )
    );

  let decodeJobInfix = {
    let ((<$>), (<*>)) = Decode.(map, apply);

    Sample.makeJob
    <$> Decode.(field("title", string))
    <*> Decode.(field("companyName", string))
    <*> Decode.(field("startDate", date))
    <*> Decode.pure(None);
  };

  let rec decodeJobPipeline = json =>
    Decode.Pipeline.(
      succeed(Sample.makeJob)
      |> field("title", string)
      |> field("companyName", string)
      |> field("startDate", date)
      |> optionalField("manager", decodeEmployeePipeline)
      |> run(json)
    )
  and decodeEmployeePipeline = json =>
    Decode.Pipeline.(
      succeed(Sample.makeEmployee)
      |> field("name", string)
      |> field("age", intFromNumber)
      |> field("job", decodeJobPipeline)
      |> run(json)
    );

  let decodeJobPipelineRecovery =
    Decode.Pipeline.(
      succeed(Sample.makeJob)
      |> hardcoded("Title")
      |> fallbackField("x", string, "Company")
      |> at(["job", "manager", "job", "startDate"], date)
      |> hardcoded(None)
    );

  test("map3", () =>
    expect(decodeJobMap3(Sample.jsonJobCeo))
    |> toEqual(Some(Sample.jobCeo))
  );

  test("lazy infix", () =>
    expect(decodeJobInfix(Sample.jsonJobCeo))
    |> toEqual(Some(Sample.jobCeo))
  );

  test("pipeline", () =>
    expect(decodeEmployeePipeline(Sample.jsonPersonBill))
    |> toEqual(Some(Sample.employeeBill))
  );

  test("pipeline hardcoded/fallback", () =>
    expect(decodeJobPipelineRecovery(Sample.jsonPersonBill))
    |> toEqual(
         Some(
           Sample.makeJob("Title", "Company", Sample.jobCeo.startDate, None),
         ),
       )
  );
});

// here we import a gigantic json file (as raw json, to avoid slowing down the
// compiler)
[@bs.module] external bigjson: Js.Json.t = "./utils/BigJson.json";

describe("Big JSON array", () =>
  test("Doesn't blow up", () =>
    expect(Decode.array(Option.pure, bigjson) |> Option.isSome)
    |> toEqual(true)
  )
);
