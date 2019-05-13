open Jest;
open Expect;
open Relude.Globals;

module Decode = Decode.AsResult.OfParseError.Pipeline;

module Point = {
  type t = {
    x: float,
    y: float,
  };
  let make = (x, y) => {x, y};
  let sample: Js.Json.t = [%raw {|
    {"x": 3.1, "y": 2}
  |}];
};

module User = {
  type t = {
    name: string,
    age: option(int),
    email: option(string),
    note: string,
  };

  let make = (name, age, email, note) => {name, age, email, note};

  let sampleComplete =
    Js.(
      Dict.fromList(
        Json.[
          ("name", string("Foo")),
          ("age", number(30.)),
          ("email", string("user@example.com")),
          ("note", string("This is a note about the user")),
        ],
      )
    )
    |> Js.Json.object_;

  let sampleSparse =
    Js.Dict.fromList([("email", Js.Json.null)]) |> Js.Json.object_;
};

describe("Test lazily executing decoders with a single JSON object", () => {
  let decoded =
    Decode.(
      succeed(Point.make)
      |> field("x", floatFromNumber)
      |> field("y", floatFromNumber)
      |> run(Point.sample)
    );

  test("Lazy execution successfully parses point", () =>
    expect(decoded) |> toEqual(Result.ok(Point.make(3.1, 2.0)))
  );
});

describe("Test piping to build up decoders, using |>", () => {
  let decoded =
    Decode.(
      succeed(Point.make)
      |> field("x", floatFromNumber)
      |> field("y", floatFromNumber)
      |> run(Point.sample)
    );

  test("Pipeline of required fields parses point", () =>
    expect(decoded) |> toEqual(Result.ok(Point.make(3.1, 2.0)))
  );
});

describe("Test optional, fallback, hardcoded helpers", () => {
  let decoder =
    Decode.(
      succeed(User.make)
      |> fallback("name", string, "Bar")
      |> optionalField("age", intFromNumber)
      |> optionalField("email", string)
      |> hardcoded("No note")
    );

  let completeName =
    Decode.map(v => v.User.name, decoder, User.sampleComplete);
  let completeAge = Decode.map(v => v.User.age, decoder, User.sampleComplete);
  let completeNote =
    decoder(User.sampleComplete) |> Result.map(v => v.User.note);

  let sparseDecoded = decoder(User.sampleSparse);
  let sparseName = sparseDecoded |> Result.map((v: User.t) => v.name);
  let sparseAge = sparseDecoded |> Result.map((v: User.t) => v.age);
  let sparseEmail = sparseDecoded |> Result.map((v: User.t) => v.email);
  let sparseNote = sparseDecoded |> Result.map((v: User.t) => v.note);

  test("Fallback ignored when field is present", () =>
    expect(completeName) |> toEqual(Result.ok("Foo"))
  );
  test("Fallback works when field is missing", () =>
    expect(sparseName) |> toEqual(Result.ok("Bar"))
  );

  test("Optional is Some when field is present", () =>
    expect(completeAge) |> toEqual(Result.ok(Some(30)))
  );
  test("Optional is None when field is missing", () =>
    expect(sparseAge) |> toEqual(Result.ok(None))
  );
  test("Optional is None when value is JSON null", () =>
    expect(sparseEmail) |> toEqual(Result.ok(None))
  );

  test("Hardcoded overrides value present in JSON", () =>
    expect(completeNote) |> toEqual(Result.ok("No note"))
  );
  test("Hardcoded is used when value is missing in JSON", () =>
    expect(sparseNote) |> toEqual(Result.ok("No note"))
  );
});
