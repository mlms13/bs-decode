open Jest;
open Expect;
open Decode;
open DecodeError;
open Belt.Result;

describe("Test primitive decoders", () => {
  let jsonString = Js.Json.string("Foo");
  let jsonFloat = Js.Json.number(1.4);
  let jsonInt: Js.Json.t = [%bs.raw {| 4 |}];
  let jsonNull = Js.Json.null;

  test("String succeeds on string", () => expect(decodeString(jsonString)) |> toEqual(Ok("Foo")));
  test("String fails on number", () => expect(decodeString(jsonFloat)) |> toEqual(Error(Primitive(ExpectedString, jsonFloat))));
  test("String fails on null", () => expect(decodeString(jsonNull)) |> toEqual(Error(Primitive(ExpectedString, jsonNull))));

  test("Float succeeds on float", () => expect(decodeFloat(jsonFloat)) |> toEqual(Ok(1.4)));
  test("Float succeeds on int", () => expect(decodeFloat(jsonInt)) |> toEqual(Ok(4.0)));
  test("Float fails on string", () => expect(decodeFloat(jsonString)) |> toEqual(Error(Primitive(ExpectedNumber, jsonString))));
  test("Float fails on null", () => expect(decodeFloat(jsonNull)) |> toEqual(Error(Primitive(ExpectedNumber, jsonNull))));

  test("Int succeeds on int", () => expect(decodeInt(jsonInt)) |> toEqual(Ok(4)));
  /* test("Int fails on float", () => expect(decodeInt(jsonFloat)) |> toEqual(Error(Primitive(Expected?, jsonFloat)))); */
  test("Int fails on string", () => expect(decodeInt(jsonString)) |> toEqual(Error(Primitive(ExpectedNumber, jsonString))));
  test("Int fails on null", () => expect(decodeInt(jsonNull)) |> toEqual(Error(Primitive(ExpectedNumber, jsonNull))));
});

describe("Test array decoders", () => {
  let jsonArray = Js.Json.array([| Js.Json.string("a"), Js.Json.string("b"), Js.Json.string("c") |]);
  let jsonEmptyArray = Js.Json.array([||]);
  let jsonString = Js.Json.string("Foo");
  let jsonArrayMixed = Js.Json.array([| Js.Json.string("a"), Js.Json.string("b"), Js.Json.number(3.5) |]);

  let numError = str => Primitive(ExpectedNumber, Js.Json.string(str));

  let decodeErrs = NonEmptyList.cons(
    (0, numError("a")), NonEmptyList.cons(
    (1, numError("b")), NonEmptyList.pure(
    (2, numError("c"))
  )));

  test("Array succeeds on array of string", () => expect(decodeArray(decodeString, jsonArray)) |> toEqual(Ok([| "a", "b", "c" |])));
  test("Array string succeeds on empty array", () => expect(decodeArray(decodeString, jsonEmptyArray)) |> toEqual(Ok([||])));
  test("Array int succeeds on empty array", () => expect(decodeArray(decodeInt, jsonEmptyArray)) |> toEqual(Ok([||])));
  test("Array fails on string", () => expect(decodeArray(decodeString, jsonString)) |> toEqual(Error(Primitive(ExpectedArray, jsonString))));
  test("Array inner decode int fails on array of string", () => expect(decodeArray(decodeInt, jsonArray)) |> toEqual(Error(Arr(decodeErrs))));
  test("Array fails on mixed array", () =>
    expect(decodeArray(decodeString, jsonArrayMixed)) |> toEqual(Error(Arr(NonEmptyList.pure((2, Primitive(ExpectedString, Js.Json.number(3.5)))))))
  );

  test("List succeeds on JSON array of string", () => expect(decodeList(decodeString, jsonArray)) |> toEqual(Ok(["a", "b", "c"])));
  test("List fails on JSON null", () => expect(decodeList(decodeString, Js.Json.null)) |> toEqual(Error(Primitive(ExpectedArray, Js.Json.null))));
  test("List inner decode int fails on array of string", () => expect(decodeList(decodeInt, jsonArray)) |> toEqual(Error(Arr(decodeErrs))));
});

describe("Test record field decoders", () => {
  let (string, number, object_) = Js.Json.(string, number, object_);
  let obj = Js.Dict.fromList([("name", string("Foo")), ("age", number(30.))]) |> object_;

  test("String field succeeds", () => expect(decodeField("name", decodeString, obj)) |> toEqual(Ok("Foo")));
  test("Int field succeeds", () => expect(decodeField("age", decodeInt, obj)) |> toEqual(Ok(30)));
  test("Field fails when missing", () => expect(decodeField("blah", decodeInt, obj)) |> toEqual(Error(objPure("blah", MissingField))));
  test("Field fails on wrong type", () => expect(decodeField("name", decodeInt, obj)) |> toEqual(Error(objPure("name", ParseField(Primitive(ExpectedNumber, string("Foo")))))));
});

describe("Test optional field and value decoders", () => {
  ();
});
