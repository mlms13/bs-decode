open Jest;
open Expect;

module D = Decode.AsOption;

let ((<$>), (<*>)) = BsAbstract.Option.Infix.((<$>), (<*>));

module User = {
  type t = {
    name: string,
    age: int,
  };
  let make = (name, age) => {name, age};

  let jsonSample =
    Js.Dict.fromList([
      ("name", Js.Json.string("Foo")),
      ("age", Js.Json.number(30.)),
    ])
    |> Js.Json.object_;

  let decode = json =>
    make
    <$> D.field("name", D.string, json)
    <*> D.field("age", D.intFromNumber, json);
};

describe("Test decoding primitive values as option", () => {
  let jsonString = Js.Json.string("Foo");
  let jsonFloat = Js.Json.number(1.4);
  let jsonInt = Js.Json.number(4.);
  let jsonNull = Js.Json.null;
  let dateString = "2018-11-17T05:40:35.869Z";
  let dateNumber = 1542433304450.0;
  let jsonDateString = Js.Json.string(dateString);
  let jsonDateNumber = Js.Json.number(dateNumber);

  test("String succeeds on string", () =>
    expect(D.string(jsonString)) |> toEqual(Some("Foo"))
  );
  test("String fails on number", () =>
    expect(D.string(jsonFloat)) |> toEqual(None)
  );
  test("String fails on null", () =>
    expect(D.string(jsonNull)) |> toEqual(None)
  );

  test("Float succeeds on float", () =>
    expect(D.floatFromNumber(jsonFloat)) |> toEqual(Some(1.4))
  );
  test("Float gives deprecated warning", () =>
    expect(D.float(jsonFloat)) |> toEqual(Some(1.4))
  );
  test("Float succeeds on int", () =>
    expect(D.floatFromNumber(jsonInt)) |> toEqual(Some(4.0))
  );
  test("Float fails on string", () =>
    expect(D.floatFromNumber(jsonString)) |> toEqual(None)
  );
  test("Float fails on null", () =>
    expect(D.floatFromNumber(jsonNull)) |> toEqual(None)
  );

  test("Int succeeds on int", () =>
    expect(D.intFromNumber(jsonInt)) |> toEqual(Some(4))
  );
  test("Int gives deprecated warning", () =>
    expect(D.int(jsonInt)) |> toEqual(Some(4))
  );
  test("Int fails on float", () =>
    expect(D.intFromNumber(jsonFloat)) |> toEqual(None)
  );
  test("Int fails on string", () =>
    expect(D.intFromNumber(jsonString)) |> toEqual(None)
  );
  test("Int fails on null", () =>
    expect(D.intFromNumber(jsonNull)) |> toEqual(None)
  );

  test("Date succeeds on number value", () =>
    expect(D.date(jsonDateNumber))
    |> toEqual(Some(dateNumber->Js.Date.fromFloat))
  );
  test("Date succeeds on string value", () =>
    expect(D.date(jsonDateString))
    |> toEqual(Some(dateString->Js.Date.fromString))
  );
  test("Date fails on an invalid date value", () =>
    expect(D.date(jsonString)) |> toEqual(None)
  );
  test("Date fails on a null value", () =>
    expect(D.date(jsonNull)) |> toEqual(None)
  );
});

[@bs.deriving jsConverter]
type color = [ | `blue | `red | `green];

[@bs.deriving jsConverter]
type numbers =
  | Zero
  | One
  | Two;

describe("Test decoding variants as option", () => {
  test("Can decode string variants", () =>
    expect(D.variantFromString(colorFromJs, "blue"->Js.Json.string))
    |> toEqual(Some(`blue))
  );
  test("Can decode number variants", () =>
    expect(D.variantFromInt(numbersFromJs, 0->float_of_int->Js.Json.number))
    |> toEqual(Some(Zero))
  );
  test("Can fail on invalid string options", () =>
    expect(D.variantFromString(colorFromJs, "yellow"->Js.Json.string))
    |> toEqual(None)
  );
  test("Can fail on invalid number options", () =>
    expect(D.variantFromInt(numbersFromJs, 5->float_of_int->Js.Json.number))
    |> toEqual(None)
  );
});

describe("Test decoding array as option", () => {
  let jsonArray =
    Js.Json.array([|
      Js.Json.string("a"),
      Js.Json.string("b"),
      Js.Json.string("c"),
    |]);
  let jsonEmptyArray = Js.Json.array([||]);
  let jsonString = Js.Json.string("Foo");

  test("Array succeeds on array of string", () =>
    expect(D.array(D.string, jsonArray))
    |> toEqual(Some([|"a", "b", "c"|]))
  );
  test("Array succeeds on empty array", () =>
    expect(D.array(D.string, jsonEmptyArray)) |> toEqual(Some([||]))
  );
  test("Array fails on string", () =>
    expect(D.array(D.string, jsonString)) |> toEqual(None)
  );
});

describe("Test decoding record as option", () => {
  let obj = User.jsonSample;

  test("String field succeeds", () =>
    expect(D.field("name", D.string, obj)) |> toEqual(Some("Foo"))
  );
  test("Field fails when missing", () =>
    expect(D.field("blah", D.string, obj)) |> toEqual(None)
  );
  test("Field fails when wrong type", () =>
    expect(D.field("name", D.floatFromNumber, obj)) |> toEqual(None)
  );
  test("Decode all fields into record type", () =>
    expect(User.decode(obj)) |> toEqual(Some(User.make("Foo", 30)))
  );
  test("Decode record fails on bad data", () =>
    expect(User.decode(Js.Json.null)) |> toEqual(None)
  );
});

describe("Test decoding optional fields and values", () => {
  let jsonNull = Js.Json.null;
  let jsonFloat = Js.Json.number(3.14);
  let jsonObj =
    Js.Dict.fromList([("float", jsonFloat), ("empty", jsonNull)])
    |> Js.Json.object_;

  let optMissingField = D.optionalField("x", D.intFromNumber, jsonObj);
  let optMissingValue = D.optionalField("empty", D.intFromNumber, jsonObj);
  let optFloatAsString = D.optionalField("float", D.string, jsonObj);

  test("Present value parses as Some", () =>
    expect(D.optional(D.floatFromNumber, jsonFloat))
    |> toEqual(Some(Some(3.14)))
  );
  test("Missing optional value parses as Some(None)", () =>
    expect(D.optional(D.floatFromNumber, jsonNull)) |> toEqual(Some(None))
  );
  test("Present field with null value parses as None for optional field", () =>
    expect(optMissingField) |> toEqual(Some(None))
  );
  test("Present field with null value parses as None for optional value", () =>
    expect(optMissingValue) |> toEqual(Some(None))
  );
  test("Incorrect field type is still rejected", () =>
    expect(optFloatAsString) |> toEqual(None)
  );
});

module Url = {
  type t =
    | Url(string);
  let make = str => Url(str);
};

describe("Test mapping options with existing Option utilities", () => {
  let url = "http://www.example.com";
  let jsonUrl = Js.Json.string(url);
  let decoded = D.string(jsonUrl)->Belt.Option.map(Url.make);

  test("Output of decoding can be mapped using existing Option tools", () =>
    expect(decoded) |> toEqual(Some(Url.make(url)))
  );
});
