/**
 * This module demonstrates how to extend the base failure type with additional
 * constructors.
 */
open Jest;
open Expect;
open Relude.Globals;

module ParseError = Decode_ParseError;
module Sample = Decode_TestSampleData;

type customError = [ ParseError.base | `InvalidColor | `InvalidShape];

[@ocaml.warning "-3"]
module ResultCustom =
  Decode.ParseError.ResultOf({
    type t = customError;
    let handle = x => (x :> t);
  });

[@ocaml.warning "-3"]
module Decode = Decode.Make(ResultCustom.TransformError, ResultCustom);

let toDebugString = (err, json) =>
  switch (err) {
  | `InvalidColor => "Expected color but found " ++ Js.Json.stringify(json)
  | `InvalidShape => "Expected shape but found " ++ Js.Json.stringify(json)
  | #ParseError.base as e => ParseError.failureToString(e, json)
  };

// helper module to represent the color type and provie a simple decoder
module Color = {
  type t =
    | Red
    | Green
    | Blue;

  let fromString =
    fun
    | "red" => Result.ok(Red)
    | "green" => Result.ok(Green)
    | "blue" => Result.ok(Blue)
    | str =>
      Result.error(ParseError.Val(`InvalidColor, Js.Json.string(str)));

  let decode =
    Decode.(
      string |> map(String.toLowerCase) |> flatMap(fromString >> const)
    );
};

describe("Color (simple string validation)", () => {
  test("Color.decode success", () =>
    expect(Color.decode(Sample.jsonStringBlue))
    |> toEqual(Result.ok(Color.Blue))
  );

  test("Color.decode failure (invalid color string)", () =>
    expect(Color.decode(Sample.jsonStringYellow))
    |> toEqual(
         Result.error(
           ParseError.Val(`InvalidColor, Sample.jsonStringYellow),
         ),
       )
  );

  test("Color.decode failure (not a string)", () =>
    expect(Color.decode(Sample.jsonNull))
    |> toEqual(
         Result.error(ParseError.Val(`ExpectedString, Sample.jsonNull)),
       )
  );
});

module Shape = {
  type t =
    | Rectangle(float, float)
    | Square(float)
    | Circle(float);

  let makeSquare = side => Square(side);
  let makeCircle = radius => Circle(radius);

  let fromKind =
    fun
    | "Rectangle" => {
        open Decode;
        let+ width = field("width", floatFromNumber)
        and+ height = field("height", floatFromNumber);
        Rectangle(width, height);
      }
    | "Square" => Decode.(field("side", floatFromNumber) |> map(makeSquare))
    | "Circle" =>
      Decode.(field("radius", floatFromNumber) |> map(makeCircle))
    | _ => (json => Result.error(ParseError.Val(`InvalidShape, json)));

  let decode = Decode.(field("kind", string) |> flatMap(fromKind));
};

describe("Shape (complex object decoding)", () => {
  test("Shape.decode (success, rectangle)", () =>
    expect(Shape.decode(Sample.jsonShapeRectangle))
    |> toEqual(Result.ok(Shape.Rectangle(3.5, 7.0)))
  );

  test("Shape.decode (success, square)", () =>
    expect(Shape.decode(Sample.jsonShapeSquare))
    |> toEqual(Result.ok(Shape.Square(4.0)))
  );

  test("Shape.decode (success, circle)", () =>
    expect(Shape.decode(Sample.jsonShapeCircle))
    |> toEqual(Result.ok(Shape.Circle(2.0)))
  );

  test("Shape.decode (failure, invalid kind)", () =>
    expect(Shape.decode(Sample.jsonShapeInvalid))
    |> toEqual(
         Result.error(
           ParseError.Val(`InvalidShape, Sample.jsonShapeInvalid),
         ),
       )
  );
});
