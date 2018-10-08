open Jest;
open Expect;
open Belt.Result;
open Decode.ParseError;

/**
 * Because the underlying failure in DecodeBase is polymorphic, you can extend
 * those errors (without mapping the error into your own type). This test file
 * provides some demonstration of how you can do this in practice.
 *
 * However, I probably won't get the API right on the first try. Or the second.
 * Until bs-decode hits 1.0, assume that this will go through some changes.
 */
module R =
  Decode.ParseError.ResultOf({
    type t = [ DecodeBase.failure | `InvalidColor | `InvalidShape];
    let handle = x => (x :> t);
  });

module D = DecodeBase.DecodeBase(R.TransformError, R.Monad, R.Alt);

let ((<$>), (>>=), (<|>)) = R.Infix.((<$>), (>>=), (<|>));

module Color = {
  type t =
    | Red
    | Green
    | Blue;

  let fromString = str =>
    switch (str) {
    | "Red" => Ok(Red)
    | "Green" => Ok(Green)
    | "Blue" => Ok(Blue)
    | _ => Error(Val(`InvalidColor, Js.Json.string(str)))
    };

  let decode = json => D.string(json) >>= fromString;
};

describe("Test decoding enum from string", () => {
  let jsonNull = Js.Json.null;
  let jsonInt = Js.Json.number(3.);
  let jsonInvalidStr = Js.Json.string("foo");
  let jsonValidStr = Js.Json.string("Blue");

  test("Enum success on valid string", () =>
    expect(Color.decode(jsonValidStr)) |> toEqual(Ok(Color.Blue))
  );
  test("Enum failure on null", () =>
    expect(Color.decode(jsonNull))
    |> toEqual(Error(Val(`ExpectedString, jsonNull)))
  );
  test("Enum failure on int", () =>
    expect(Color.decode(jsonInt))
    |> toEqual(Error(Val(`ExpectedString, jsonInt)))
  );
  test("Enum failure on invalid string", () =>
    expect(Color.decode(jsonInvalidStr))
    |> toEqual(Error(Val(`InvalidColor, jsonInvalidStr)))
  );
});

module Shape = {
  type t =
    | Circle(float)
    | Square(float)
    | Rect(float, float);

  let circle = radius => Circle(radius);
  let square = length => Square(length);
  let rect = ((width, height)) => Rect(width, height);

  let decodeWH = json =>
    D.tuple(("width", D.float), ("height", D.float), json);

  /* e.g. { "circle": { "radius": 1.4 }} */
  let decode = json =>
    circle
    <$> D.at(["circle", "radius"], D.float, json)
    <|> (square <$> D.at(["square", "length"], D.float, json))
    <|> (rect <$> D.field("rectangle", decodeWH, json))
    |> R.mapErr(_ => `InvalidShape(json));
};

describe("Test decoding complex ADT from object", () => {
  let objOfList = lst => Js.Dict.fromList(lst) |> Js.Json.object_;
  let jsonNull = Js.Json.null;

  /* { "circle": { "radius": 3.2 }} */
  let circleAsKey =
    objOfList([("circle", objOfList([("radius", Js.Json.number(3.2))]))]);

  /* { "rect" : { "width": 2.1, "height": 3.4 }} */
  let rectAsKey =
    objOfList([
      (
        "rectangle",
        objOfList([
          ("height", Js.Json.number(3.4)),
          ("width", Js.Json.number(2.1)),
        ]),
      ),
    ]);

  /* { "square": { length: "foo" }} */
  let invalidNested =
    objOfList([
      ("square", objOfList([("length", Js.Json.string("foo"))])),
    ]);

  /* { "square": "foo" } */
  let invalidVal = objOfList([("square", Js.Json.string("foo"))]);

  test("ADT success on valid circle", () =>
    expect(Shape.decode(circleAsKey)) |> toEqual(Ok(Shape.circle(3.2)))
  );
  test("ADT success on valid rectangle", () =>
    expect(Shape.decode(rectAsKey)) |> toEqual(Ok(Shape.rect((2.1, 3.4))))
  );
  test("ADT failure on invalid square length", () =>
    expect(Shape.decode(invalidNested))
    |> toEqual(Error(`InvalidShape(invalidNested)))
  );
  test("ADT failure on invalid square value type", () =>
    expect(Shape.decode(invalidVal))
    |> toEqual(Error(`InvalidShape(invalidVal)))
  );
  test("ADT failure on JSON null", () =>
    expect(Shape.decode(jsonNull))
    |> toEqual(Error(`InvalidShape(jsonNull)))
  );
});

describe("Test decoding complex ADT with constructor as JSON value", () => {
  /**
   * Now imagine we encode our Shape into JSON like this:
   * { key: "circle", value: { radius: 3.0 }}
   *
   */
  let decodeShape = json =>
    D.field("key", D.string, json)
    ->(
        flatMap(key =>
          switch (key) {
          | "circle" =>
            Shape.circle <$> D.at(["value", "radius"], D.float, json)
          | "square" =>
            Shape.square <$> D.at(["value", "length"], D.float, json)
          | "rectangle" =>
            Shape.rect <$> D.field("value", Shape.decodeWH, json)
          | _ => Error(Val(`InvalidShape, json)) /* This is a sad lie to make the compiler happy */
          }
        )
      );

  let valid =
    Js.Dict.fromList([
      ("key", Js.Json.string("rectangle")),
      (
        "value",
        Js.Dict.fromList([
          ("width", Js.Json.number(3.0)),
          ("height", Js.Json.number(4.0)),
        ])
        |> Js.Json.object_,
      ),
    ])
    |> Js.Json.object_;

  let invalid =
    Js.Dict.fromList([("key", Js.Json.string("foo"))]) |> Js.Json.object_;

  let emptyObj = Js.Dict.fromList([]) |> Js.Json.object_;

  test("ADT success on valid rectangle", () =>
    expect(decodeShape(valid)) |> toEqual(Ok(Shape.rect((3.0, 4.0))))
  );
  test("ADT failure on invalid key", () =>
    expect(decodeShape(invalid))
    |> toEqual(Error(Val(`InvalidShape, invalid)))
  );
  test("ADT failure on empty object", () =>
    expect(decodeShape(emptyObj))
    |> toEqual(Error(objPure("key", MissingField)))
  );
});
