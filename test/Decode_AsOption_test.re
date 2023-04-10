open Jest;
open Expect;
open Relude.Globals;

[@ocaml.warning "-3"]
module Decode = Decode.AsOption;
module Sample = Decode_TestSampleData;

describe("Nested decoders", () => {
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
