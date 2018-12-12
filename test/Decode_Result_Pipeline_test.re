open Jest;
open Expect;
open Belt.Result;

module D = Decode.AsResult.OfParseError;

let (string, number, object_, null) =
  Js.Json.(string, number, object_, null);

let (succeed, map2, field, fallback, optionalField, hardcoded, run) =
  D.Pipeline.(succeed, map2, field, fallback, optionalField, hardcoded, run);

let map = D.ResultUtil.Functor.map;

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
    map2(
      Point.make,
      D.field("x", D.floatFromNumber),
      D.field("y", D.floatFromNumber),
    )
    |> run(Point.sample);

  test("Lazy execution successfully parses point", () =>
    expect(decoded) |> toEqual(Ok(Point.make(3.1, 2.0)))
  );
});

describe("Test piping to build up decoders, using |>", () => {
  let decoded =
    succeed(Point.make)
    |> field("x", D.floatFromNumber)
    |> field("y", D.floatFromNumber)
    |> run(Point.sample);

  test("Pipeline of required fields parses point", () =>
    expect(decoded) |> toEqual(Ok(Point.make(3.1, 2.0)))
  );
});

describe("Test optional, fallback, hardcoded helpers", () => {
  let decoder =
    succeed(User.make)
    |> fallback("name", D.string, "Bar")
    |> optionalField("age", D.intFromNumber)
    |> optionalField("email", D.string)
    |> hardcoded("No note");

  let completeDecoded = decoder |> run(User.sampleComplete);
  let completeName = completeDecoded |> map((v: User.t) => v.name);
  let completeAge = completeDecoded |> map((v: User.t) => v.age);
  let completeNote = completeDecoded |> map((v: User.t) => v.note);

  let sparseDecoded = decoder |> run(User.sampleSparse);
  let sparseName = sparseDecoded |> map((v: User.t) => v.name);
  let sparseAge = sparseDecoded |> map((v: User.t) => v.age);
  let sparseEmail = sparseDecoded |> map((v: User.t) => v.email);
  let sparseNote = sparseDecoded |> map((v: User.t) => v.note);

  test("Fallback ignored when field is present", () =>
    expect(completeName) |> toEqual(Ok("Foo"))
  );
  test("Fallback works when field is missing", () =>
    expect(sparseName) |> toEqual(Ok("Bar"))
  );

  test("Optional is Some when field is present", () =>
    expect(completeAge) |> toEqual(Ok(Some(30)))
  );
  test("Optional is None when field is missing", () =>
    expect(sparseAge) |> toEqual(Ok(None))
  );
  test("Optional is None when value is JSON null", () =>
    expect(sparseEmail) |> toEqual(Ok(None))
  );

  test("Hardcoded overrides value present in JSON", () =>
    expect(completeNote) |> toEqual(Ok("No note"))
  );
  test("Hardcoded is used when value is missing in JSON", () =>
    expect(sparseNote) |> toEqual(Ok("No note"))
  );
});
