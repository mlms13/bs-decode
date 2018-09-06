open Jest;
open Expect;
open Belt.Result;

let (number, object_) = Js.Json.(number, object_);
let (succeed, map2, required, run) = Decode_Pipeline.(succeed, map2, required, run);

module Point = {
  type t = { x: float, y: float };
  let make = (x, y) => { x, y };
};

let pointJSON = Js.Dict.fromList([("x", number(3.1)), ("y", number(2.))]) |> object_;

describe("Test lazily executing decoders with a single JSON object", () => {
  let decoded =
    map2(
      Point.make,
      Decode.decodeField("x", Decode.decodeFloat),
      Decode.decodeField("y", Decode.decodeFloat)
    ) |. run(pointJSON);

  test("Lazy execution successfully parses point", () => expect(decoded) |> toEqual(Ok(Point.make(3.1, 2.0))));
});

describe("Test piping to build up decoders, using |>", () => {
  let decoded =
    succeed(Point.make)
      |> required("x", Decode.decodeFloat)
      |> required("y", Decode.decodeFloat)
      |. run(pointJSON);

  test("Pipeline of required fields parses point", () => expect(decoded) |> toEqual(Ok(Point.make(3.1, 2.0))));
});
