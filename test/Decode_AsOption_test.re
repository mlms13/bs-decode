open Jest;
open Expect;
open Relude.Globals;

[@ocaml.warning "-3"]
module Decode = Decode.AsOption;
module Sample = Decode_TestSampleData;

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
    expect(Decode.(optional(floatFromNumber, Sample.jsonTrue)))
    |> toEqual(None)
  );

  test("dict (succeeds)", () =>
    expect(Decode.(dict(floatFromNumber, Sample.jsonDictFloat)))
    |> toEqual(Some(Sample.valDictFloat))
  );

  test("dict (fails on record)", () =>
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

  let ((<$>), (<*>)) = Decode.(map, apply);
  let decodeJobInfix =
    Sample.makeJob
    <$> Decode.(field("title", string))
    <*> Decode.(field("companyName", string))
    <*> Decode.(field("startDate", date))
    <*> Decode.pure(None);

  [@ocaml.warning "-3"]
  let rec decodeJobPipeline = json =>
    Decode.Pipeline.(
      succeed(Sample.makeJob)
      |> field("title", string)
      |> field("companyName", string)
      |> field("startDate", date)
      |> optionalField("manager", decodeEmployeePipeline)
      |> run(json)
    )
  [@ocaml.warning "-3"]
  and decodeEmployeePipeline = json =>
    Decode.Pipeline.(
      succeed(Sample.makeEmployee)
      |> field("name", string)
      |> field("age", intFromNumber)
      |> field("job", decodeJobPipeline)
      |> run(json)
    );

  [@ocaml.warning "-3"]
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
  test("is stack-safe", () =>
    expect(Decode.array(Option.pure, bigjson) |> Option.isSome)
    |> toEqual(true)
  )
);
