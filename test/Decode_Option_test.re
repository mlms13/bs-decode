open Jest;
open Expect;

module D = Decode.AsOption;

let ((<$>), (<*>)) = BsAbstract.Option.Infix.((<$>), (<*>));

module User = {
  type t= { name: string, age: int };
  let make = (name, age) => { name, age };

  let jsonSample = Js.Dict.fromList([
    ("name", Js.Json.string("Foo")),
    ("age", Js.Json.number(30.))
  ]) |> Js.Json.object_;

  let decode = json => make
    <$> D.field("name", D.string, json)
    <*> D.field("age", D.int, json);
};

describe("Test decoding primitive values as option", () => {
  let jsonString = Js.Json.string("Foo");
  let jsonFloat = Js.Json.number(1.4);
  let jsonInt = Js.Json.number(4.);
  let jsonNull = Js.Json.null;

  test("String succeeds on string", () => expect(D.string(jsonString)) |> toEqual(Some("Foo")));
  test("String fails on number", () => expect(D.string(jsonFloat)) |> toEqual(None));
  test("String fails on null", () => expect(D.string(jsonNull)) |> toEqual(None));

  test("Float succeeds on float", () => expect(D.float(jsonFloat)) |> toEqual(Some(1.4)));
  test("Float succeeds on int", () => expect(D.float(jsonInt)) |> toEqual(Some(4.0)));
  test("Float fails on string", () => expect(D.float(jsonString)) |> toEqual(None));
  test("Float fails on null", () => expect(D.float(jsonNull)) |> toEqual(None));

  test("Int succeeds on int", () => expect(D.int(jsonInt)) |> toEqual(Some(4)));
  test("Int fails on float", () => expect(D.int(jsonFloat)) |> toEqual(None));
  test("Int fails on string", () => expect(D.int(jsonString)) |> toEqual(None));
  test("Int fails on null", () => expect(D.int(jsonNull)) |> toEqual(None));
});

describe("Test decoding array as option", () => {
  let jsonArray = Js.Json.array([| Js.Json.string("a"), Js.Json.string("b"), Js.Json.string("c") |]);
  let jsonEmptyArray = Js.Json.array([||]);
  let jsonString = Js.Json.string("Foo");

  test("Array succeeds on array of string", () => expect(D.array(D.string, jsonArray)) |> toEqual(Some([| "a", "b", "c" |])));
  test("Array succeeds on empty array", () => expect(D.array(D.string, jsonEmptyArray)) |> toEqual(Some([||])));
  test("Array fails on string", () => expect(D.array(D.string, jsonString)) |> toEqual(None));
});

describe("Test decoding record as option", () => {
  let obj = User.jsonSample;

  test("String field succeeds", () => expect(D.field("name", D.string, obj)) |> toEqual(Some("Foo")));
  test("Field fails when missing", () => expect(D.field("blah", D.string, obj)) |> toEqual(None));
  test("Field fails when wrong type", () => expect(D.field("name", D.float, obj)) |> toEqual(None));
  test("Decode all fields into record type", () => expect(User.decode(obj)) |> toEqual(Some(User.make("Foo", 30))));
  test("Decode record fails on bad data", () => expect(User.decode(Js.Json.null)) |> toEqual(None));
});

describe("Test decoding optional fields and values", () => {
  let jsonNull = Js.Json.null;
  let jsonFloat = Js.Json.number(3.14);
  let jsonObj = Js.Dict.fromList([
    ("float", jsonFloat),
    ("empty", jsonNull)
  ]) |> Js.Json.object_;

  let optMissingField = D.optionalField("x", D.int, jsonObj);
  let optMissingValue = D.optionalField("empty", D.int, jsonObj);
  let optFloatAsString = D.optionalField("float", D.string, jsonObj);

  test("Present value parses as Some", () => expect(D.optional(D.float, jsonFloat)) |> toEqual(Some(Some(3.14))));
  test("Missing optional value parses as Some(None)", () => expect(D.optional(D.float, jsonNull)) |> toEqual(Some(None)));
  test("Present field with null value parses as None for optional field", () => expect(optMissingField) |> toEqual(Some(None)));
  test("Present field with null value parses as None for optional value", () => expect(optMissingValue) |> toEqual(Some(None)));
  test("Incorrect field type is still rejected", () => expect(optFloatAsString) |> toEqual(None));
});
