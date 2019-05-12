open Relude.Globals;
open Jest;
open Expect;
open Decode.ParseError;

module D = Decode.AsResult.OfParseError;
module Nel = D.NonEmptyList;

/**
 * Sample module used to decode json into a record type
 */
module User = {
  let ((<$>), (<*>)) = (Result.map, Result.apply);
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

module Parent = {
  let ((<$>), (<*>)) = (Result.map, Result.apply);
  type t = {
    user: User.t,
    other: float,
  };

  let make = (user, other) => {user, other};

  let sameplJson =
    Js.(Dict.fromList([("user", User.jsonSample)]) |> Json.object_);

  let decode = obj =>
    make <$> D.field("user", User.decode, obj) <*> Result.ok(3.14);
};

describe("Test value decoders", () => {
  let jsonBoolean = Js.Json.boolean(true);
  let jsonString = Js.Json.string("Foo");
  let jsonFloat = Js.Json.number(1.4);
  let jsonInt: Js.Json.t = [%bs.raw {| 4 |}];
  let jsonZero: Js.Json.t = [%bs.raw {| 0 |}];
  let jsonNull = Js.Json.null;
  let dateString = "2018-11-17T05:40:35.869Z";
  let dateNumber = 1542433304450.0;
  let jsonDateString = Js.Json.string(dateString);
  let jsonDateNumber = Js.Json.number(dateNumber);

  test("Boolean succeeds on a boolean", () =>
    expect(D.boolean(jsonBoolean)) |> toEqual(Result.ok(true))
  );
  test("Boolean fails on string", () =>
    expect(D.boolean(jsonString))
    |> toEqual(Result.error(Val(`ExpectedBoolean, jsonString)))
  );
  test("Boolean fails on number", () =>
    expect(D.boolean(jsonFloat))
    |> toEqual(Result.error(Val(`ExpectedBoolean, jsonFloat)))
  );
  test("Boolean fails on null", () =>
    expect(D.boolean(jsonNull))
    |> toEqual(Result.error(Val(`ExpectedBoolean, jsonNull)))
  );

  test("String succeeds on string", () =>
    expect(D.string(jsonString)) |> toEqual(Result.ok("Foo"))
  );
  test("String fails on boolean", () =>
    expect(D.string(jsonBoolean))
    |> toEqual(Result.error(Val(`ExpectedString, jsonBoolean)))
  );
  test("String fails on number", () =>
    expect(D.string(jsonFloat))
    |> toEqual(Result.error(Val(`ExpectedString, jsonFloat)))
  );
  test("String fails on null", () =>
    expect(D.string(jsonNull))
    |> toEqual(Result.error(Val(`ExpectedString, jsonNull)))
  );

  test("Float succeeds on float", () =>
    expect(D.floatFromNumber(jsonFloat)) |> toEqual(Result.ok(1.4))
  );
  test("Float gives deprecated warning", () =>
    expect(D.floatFromNumber(jsonFloat)) |> toEqual(Result.ok(1.4))
  );
  test("Float succeeds on int", () =>
    expect(D.floatFromNumber(jsonInt)) |> toEqual(Result.ok(4.0))
  );
  test("Float succeeds on zero", () =>
    expect(D.floatFromNumber(jsonZero)) |> toEqual(Result.ok(0.))
  );
  test("Float fails on boolean", () =>
    expect(D.floatFromNumber(jsonBoolean))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonBoolean)))
  );
  test("Float fails on string", () =>
    expect(D.floatFromNumber(jsonString))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonString)))
  );
  test("Float fails on null", () =>
    expect(D.floatFromNumber(jsonNull))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonNull)))
  );

  test("Int succeeds on int", () =>
    expect(D.intFromNumber(jsonInt)) |> toEqual(Result.ok(4))
  );
  test("Int gives deprecation warning", () =>
    expect(D.intFromNumber(jsonInt)) |> toEqual(Result.ok(4))
  );
  test("Int succeeds on 0", () =>
    expect(D.intFromNumber(jsonZero)) |> toEqual(Result.ok(0))
  );
  test("Int fails on float", () =>
    expect(D.intFromNumber(jsonFloat))
    |> toEqual(Result.error(Val(`ExpectedInt, jsonFloat)))
  );
  test("Int fails on boolean", () =>
    expect(D.intFromNumber(jsonBoolean))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonBoolean)))
  );
  test("Int fails on string", () =>
    expect(D.intFromNumber(jsonString))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonString)))
  );
  test("Int fails on null", () =>
    expect(D.intFromNumber(jsonNull))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonNull)))
  );

  test("Date succeeds on number value", () =>
    expect(D.date(jsonDateNumber))
    |> toEqual(Result.ok(Js.Date.fromFloat(dateNumber)))
  );
  test("Date succeeds on string value", () =>
    expect(D.date(jsonDateString))
    |> toEqual(Result.ok(Js.Date.fromString(dateString)))
  );
  test("Date fails on an invalid date value", () =>
    expect(D.date(jsonString))
    |> toEqual(Result.error(Val(`ExpectedValidDate, jsonString)))
  );
  test("Date fails on a null value", () =>
    expect(D.date(jsonNull))
    |> toEqual(Result.error(Val(`ExpectedString, jsonNull)))
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
    expect(D.variantFromString(colorFromJs, Js.Json.string("blue")))
    |> toEqual(Result.ok(`blue))
  );
  test("Can decode number variants", () =>
    expect(D.variantFromInt(numbersFromJs, Js.Json.number(0.0)))
    |> toEqual(Result.ok(Zero))
  );
  test("Can fail on invalid string options", () =>
    expect(D.variantFromString(colorFromJs, Js.Json.string("yellow")))
    |> toEqual(
         Result.error(Val(`ExpectedValidOption, Js.Json.string("yellow"))),
       )
  );
  test("Can fail on invalid number options", () =>
    expect(D.variantFromInt(numbersFromJs, Js.Json.number(5.0)))
    |> toEqual(
         Result.error(Val(`ExpectedValidOption, Js.Json.number(5.0))),
       )
  );
});

describe("Test array decoders", () => {
  let jsonArray =
    Js.Json.(array([|string("a"), string("b"), string("c")|]));
  let jsonEmptyArray = Js.Json.array([||]);
  let jsonString = Js.Json.string("Foo");
  let jsonArrayMixed =
    Js.Json.(array([|string("a"), string("b"), number(3.5)|]));

  let numError = str => Val(`ExpectedNumber, Js.Json.string(str));

  let decodeErrs =
    Nel.make(
      (0, numError("a")),
      [(1, numError("b")), (2, numError("c"))],
    );

  test("Array succeeds on array of string", () =>
    expect(D.array(D.string, jsonArray))
    |> toEqual(Result.ok([|"a", "b", "c"|]))
  );
  test("Array string succeeds on empty array", () =>
    expect(D.array(D.string, jsonEmptyArray)) |> toEqual(Result.ok([||]))
  );
  test("Array int succeeds on empty array", () =>
    expect(D.array(D.intFromNumber, jsonEmptyArray))
    |> toEqual(Result.ok([||]))
  );
  test("Array fails on string", () =>
    expect(D.array(D.string, jsonString))
    |> toEqual(Result.error(Val(`ExpectedArray, jsonString)))
  );
  test("Array inner decode int fails on array of string", () =>
    expect(D.array(D.intFromNumber, jsonArray))
    |> toEqual(Result.error(Arr(decodeErrs)))
  );
  test("Array fails on mixed array", () =>
    expect(D.array(D.string, jsonArrayMixed))
    |> toEqual(
         Result.error(
           Arr(Nel.pure((2, Val(`ExpectedString, Js.Json.number(3.5))))),
         ),
       )
  );

  test("List succeeds on JSON array of string", () =>
    expect(D.list(D.string, jsonArray))
    |> toEqual(Result.ok(["a", "b", "c"]))
  );
  test("List fails on JSON null", () =>
    expect(D.list(D.string, Js.Json.null))
    |> toEqual(Result.error(Val(`ExpectedArray, Js.Json.null)))
  );
  test("List inner decode int fails on array of string", () =>
    expect(D.list(D.intFromNumber, jsonArray))
    |> toEqual(Result.error(Arr(decodeErrs)))
  );
});

describe("Test record field decoders", () => {
  let nameAsIntError =
    InvalidField(Val(`ExpectedNumber, Js.Json.string("Foo")));

  let decodeFail =
    D.map2(
      User.make,
      D.field("missing", D.string),
      D.field("name", D.intFromNumber),
      User.jsonSample,
    );

  // TODO: why does this seem to put the errors in the opposite order?
  // let decodeFail =
  //   D.Pipeline.(
  //     succeed(User.make)
  //     |> field("missing", D.string)
  //     |> field("name", D.intFromNumber)
  //     |> run(User.jsonSample)
  //   );

  let decodeErrors =
    Obj(Nel.make(("missing", MissingField), [("name", nameAsIntError)]))
    |> Result.error;

  test("String field succeeds", () =>
    expect(D.field("name", D.string, User.jsonSample))
    |> toEqual(Result.ok("Foo"))
  );
  test("Int field succeeds", () =>
    expect(D.field("age", D.intFromNumber, User.jsonSample))
    |> toEqual(Result.ok(30))
  );
  test("Field fails when missing", () =>
    expect(D.field("blah", D.intFromNumber, User.jsonSample))
    |> toEqual(Result.error(objPure("blah", MissingField)))
  );
  test("Field fails on wrong type", () =>
    expect(D.field("name", D.intFromNumber, User.jsonSample))
    |> toEqual(Result.error(objPure("name", nameAsIntError)))
  );
  test("Decode all fields of user record into User", () =>
    expect(User.decode(User.jsonSample))
    |> toEqual(Result.ok(User.make("Foo", 30)))
  );
  test("Decode as user with incorrect fields fails", () =>
    expect(decodeFail) |> toEqual(decodeErrors)
  );
  test("Decode parent record with child record field", () =>
    expect(Parent.decode(Parent.sameplJson))
    |> toEqual(Result.ok(Parent.make(User.make("Foo", 30), 3.14)))
  );
});

describe("Test value and field recovery from failed parse", () => {
  let jsonNumber = Js.Json.number(3.14);
  let successParse = D.floatFromNumber(jsonNumber);
  let failedParse = D.string(jsonNumber);

  let invalidUserObj =
    Js.Dict.fromList([("name", jsonNumber), ("age", Js.Json.number(30.))])
    |> Js.Json.object_;

  let objRecovery =
    D.map2(
      User.make,
      D.fallback("name", D.string, "Foo"),
      D.field("age", D.intFromNumber),
      invalidUserObj,
    );

  test("Failed parse can be recovered with literal value", () =>
    expect(failedParse |> Result.recover("foo"))
    |> toEqual(Result.ok("foo"))
  );
  test("Successful parse is unaffected by recovery", () =>
    expect(successParse |> Result.recover(1.0)) |> toEqual(Result.ok(3.14))
  );
  test("Bad field recovers with fallback", () =>
    expect(objRecovery) |> toEqual(Result.ok(User.make("Foo", 30)))
  );
});

describe("Test optionally empty fields and values", () => {
  let jsonNull = Js.Json.null;
  let jsonString = Js.Json.string("Foo");
  let jsonInt = Js.Json.number(3.);

  let optStr = D.optional(D.string);

  let obj =
    Js.Dict.fromList([("i", jsonInt), ("s", jsonString), ("n", jsonNull)])
    |> Js.Json.object_;

  let optFieldString = D.optionalField("s", D.string, obj);
  let reqFieldOptValueString = D.field("s", optStr, obj);
  let optFieldInt = D.optionalField("x", D.intFromNumber, obj);
  let optValueInt = D.field("x", D.optional(D.intFromNumber), obj);
  let nullFieldInt = D.optionalField("n", D.intFromNumber, obj);
  let nullValueInt = D.field("n", D.optional(D.intFromNumber), obj);

  test("Present value parses as Some string", () =>
    expect(optStr(jsonString)) |> toEqual(Result.ok(Some("Foo")))
  );
  test("Null value parses as None", () =>
    expect(optStr(jsonNull)) |> toEqual(Result.ok(None))
  );
  test("Present value of incorrect type is still an Error", () =>
    expect(optStr(jsonInt))
    |> toEqual(Result.error(Val(`ExpectedString, jsonInt)))
  );

  test("Present field and value is Some when field is optional", () =>
    expect(optFieldString) |> toEqual(Result.ok(Some("Foo")))
  );
  test("Present field and value is Some when value is optional", () =>
    expect(reqFieldOptValueString) |> toEqual(Result.ok(Some("Foo")))
  );
  test("If int field is missing, it parses as None if field is optional", () =>
    expect(optFieldInt) |> toEqual(Result.ok(None))
  );
  test(
    "If int field is missing, it parses as Error if only value is optional", () =>
    expect(optValueInt)
    |> toEqual(Result.error(objPure("x", MissingField)))
  );
  test("Present field with null value parses as None for optional field", () =>
    expect(nullFieldInt) |> toEqual(Result.ok(None))
  );
  test("Present field with null value parses as None for optional value", () =>
    expect(nullValueInt) |> toEqual(Result.ok(None))
  );
  test("Trying to parse string field as int is Error, not None", () =>
    expect(D.optionalField("s", D.intFromNumber, obj))
    |> toEqual(Result.error(Val(`ExpectedNumber, jsonString)))
  );
});

type kind =
  | S(string)
  | N(option(float))
  | B(bool);

let makeS = s => S(s);
let makeN = n => N(n);
let makeB = b => B(b);

describe("Test oneOf, trying multiple decoders", () => {
  let json: Js.Json.t = [%bs.raw
    {|
     {
       "strField": "string",
       "numField": 3.14,
       "boolField": false
     }
  |}
  ];

  let decoders =
    D.oneOf(
      json => Result.map(makeS, D.string(json)),
      [
        json => Result.map(makeN, D.optional(D.floatFromNumber, json)),
        json => Result.map(makeB, D.boolean(json)),
      ],
    );

  let badDecoders =
    D.oneOf(
      json => Result.map(makeS, D.string(json)),
      [
        json => Result.map(makeS, D.string(json)),
        json => Result.map(makeS, D.string(json)),
      ],
    );

  test("First in oneOf list succeeds", () =>
    expect(D.field("strField", decoders, json))
    |> toEqual(Result.ok(S("string")))
  );

  test("Last in oneOf list succeeds", () =>
    expect(D.field("boolField", decoders, json))
    |> toEqual(Result.ok(B(false)))
  );

  test("All decoders in oneOf fail", () =>
    expect(badDecoders(Js.Json.boolean(false)))
    |> toEqual(Result.error(Val(`ExpectedString, Js.Json.boolean(false))))
  );
});
