open Jest;
open Expect;
open Relude.Globals;

module D = Decode.AsResult.OfParseError;

let (string, number, object_, null) =
  Js.Json.(string, number, object_, null);

let (succeed, field, fallback, optionalField, hardcoded, run) =
  D.Pipeline.(succeed, field, fallback, optionalField, hardcoded, run);

module Point = {
  type t = {
    x: float,
    y: float,
  };
  let make = (x, y) => {x, y};
  let sample =
    Js.Dict.fromList([("x", number(3.1)), ("y", number(2.))]) |> object_;
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
    Js.Dict.fromList([
      ("name", string("Foo")),
      ("age", number(30.)),
      ("email", string("user@example.com")),
      ("note", string("This is a note about the user")),
    ])
    |> object_;

  let sampleSparse = Js.Dict.fromList([("email", null)]) |> object_;
};

describe("Test lazily executing decoders with a single JSON object", () => {
  let decoded =
    D.map2(
      Point.make,
      D.field("x", D.floatFromNumber),
      D.field("y", D.floatFromNumber),
    )
    |> run(Point.sample);

  test("Lazy execution successfully parses point", () =>
    expect(decoded) |> toEqual(Result.ok(Point.make(3.1, 2.0)))
  );
});

describe("Test piping to build up decoders, using |>", () => {
  let decoded =
    succeed(Point.make)
    |> field("x", D.floatFromNumber)
    |> field("y", D.floatFromNumber)
    |> run(Point.sample);

  test("Pipeline of required fields parses point", () =>
    expect(decoded) |> toEqual(Result.ok(Point.make(3.1, 2.0)))
  );
});

describe("Test optional, fallback, hardcoded helpers", () => {
  let decoder =
    succeed(User.make)
    |> fallback("name", D.string, "Bar")
    |> optionalField("age", D.intFromNumber)
    |> optionalField("email", D.string)
    |> hardcoded("No note");

  let completeName = D.map(v => v.User.name, decoder, User.sampleComplete);
  let completeAge = D.map(v => v.User.age, decoder, User.sampleComplete);
  let completeNote =
    decoder(User.sampleComplete) |> Result.map(v => v.User.note);

  let sparseDecoded = decoder |> run(User.sampleSparse);
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
