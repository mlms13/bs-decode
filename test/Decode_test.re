open Jest;
open Expect;

describe("Test primitive decoders", () => {
  open Decode;
  open DecodeError;
  open Belt.Result;
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