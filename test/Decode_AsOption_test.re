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
  let ((<$>), (<*>)) = Decode.(map, apply);
  let decodeJobInfix =
    Sample.makeJob
    <$> Decode.(field("title", string))
    <*> Decode.(field("companyName", string))
    <*> Decode.(field("startDate", date))
    <*> Decode.pure(None);

  test("lazy infix", () =>
    expect(decodeJobInfix(Sample.jsonJobCeo))
    |> toEqual(Some(Sample.jobCeo))
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
