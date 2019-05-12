open Relude.Globals;

open Jest;
open Expect;
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
  ResultOf({
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
    | "Red" => Result.ok(Red)
    | "Green" => Result.ok(Green)
    | "Blue" => Result.ok(Blue)
    | _ => Result.error(Val(`InvalidColor, Js.Json.string(str)))
    };

  let decode = json => D.string(json) >>= fromString;
};

describe("Test decoding enum from string", () => {
  let jsonNull = Js.Json.null;
  let jsonInt = Js.Json.number(3.);
  let jsonInvalidStr = Js.Json.string("foo");
  let jsonValidStr = Js.Json.string("Blue");

  test("Enum success on valid string", () =>
    expect(Color.decode(jsonValidStr)) |> toEqual(Result.ok(Color.Blue))
  );
  test("Enum failure on null", () =>
    expect(Color.decode(jsonNull))
    |> toEqual(Result.error(Val(`ExpectedString, jsonNull)))
  );
  test("Enum failure on int", () =>
    expect(Color.decode(jsonInt))
    |> toEqual(Result.error(Val(`ExpectedString, jsonInt)))
  );
  test("Enum failure on invalid string", () =>
    expect(Color.decode(jsonInvalidStr))
    |> toEqual(Result.error(Val(`InvalidColor, jsonInvalidStr)))
  );
  test("Enum failure can be mapped to string", () => {
    let message =
      switch (Color.decode(jsonInvalidStr)) {
      | Ok(_) as ok => ok
      | Error(parseError) =>
        Decode.ParseError.toDebugString(
          x =>
            switch (x) {
            | `InvalidColor => (_json => "Invalid color")
            | `InvalidShape => (_json => "Invalid shape")
            | #DecodeBase.failure as f => DecodeBase.failureToString(f)
            },
          parseError,
        )
        |> Result.error
      };
    expect(message) |> toEqual(Result.error("Invalid color"));
  });
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
    D.tuple(
      ("width", D.floatFromNumber),
      ("height", D.floatFromNumber),
      json,
    );

  /* e.g. { "circle": { "radius": 1.4 }} */
  let decode = json =>
    circle
    <$> D.at(["circle", "radius"], D.floatFromNumber, json)
    <|> (square <$> D.at(["square", "length"], D.floatFromNumber, json))
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
    expect(Shape.decode(circleAsKey))
    |> toEqual(Result.ok(Shape.circle(3.2)))
  );
  test("ADT success on valid rectangle", () =>
    expect(Shape.decode(rectAsKey))
    |> toEqual(Result.ok(Shape.rect((2.1, 3.4))))
  );
  test("ADT failure on invalid square length", () =>
    expect(Shape.decode(invalidNested))
    |> toEqual(Result.error(`InvalidShape(invalidNested)))
  );
  test("ADT failure on invalid square value type", () =>
    expect(Shape.decode(invalidVal))
    |> toEqual(Result.error(`InvalidShape(invalidVal)))
  );
  test("ADT failure on JSON null", () =>
    expect(Shape.decode(jsonNull))
    |> toEqual(Result.error(`InvalidShape(jsonNull)))
  );
});

describe("Test decoding complex ADT with constructor as JSON value", () => {
  let shapeFromKeyJson = (key, json) =>
    switch (key) {
    | "circle" =>
      Shape.circle <$> D.at(["value", "radius"], D.floatFromNumber, json)
    | "square" =>
      Shape.square <$> D.at(["value", "length"], D.floatFromNumber, json)
    | "rectangle" => Shape.rect <$> D.field("value", Shape.decodeWH, json)
    | other => Error(Val(`InvalidShape, Js.Json.string(other)))
    };

  /**
   * Now imagine we encode our Shape into JSON like this:
   * { key: "circle", value: { radius: 3.0 }}
   */
  let valid: Js.Json.t = [%bs.raw
    {|
    {
      "key": "rectangle",
      "value": {
        "width": 3,
        "height": 4.0
      }
    }
  |}
  ];

  let decodeShape = D.(field("key", string) |> flatMap(shapeFromKeyJson));
  let shapePipeDecode = D.Pipeline.(succeed(id) |> pipe(decodeShape));

  let invalid =
    Js.Dict.fromList([("key", Js.Json.string("foo"))]) |> Js.Json.object_;

  let emptyObj = Js.Dict.fromList([]) |> Js.Json.object_;

  test("ADT success on valid rectangle", () =>
    expect(decodeShape(valid))
    |> toEqual(Result.ok(Shape.rect((3.0, 4.0))))
  );
  test("ADT failure on invalid key", () =>
    expect(decodeShape(invalid))
    |> toEqual(Result.error(Val(`InvalidShape, Js.Json.string("foo"))))
  );
  test("ADT failure on empty object", () =>
    expect(decodeShape(emptyObj))
    |> toEqual(Result.error(objPure("key", MissingField)))
  );
  test("ADT succeeds when using `pipe` in pipeline", () =>
    expect(shapePipeDecode(valid))
    |> toEqual(Result.ok(Shape.rect((3.0, 4.0))))
  );
});
