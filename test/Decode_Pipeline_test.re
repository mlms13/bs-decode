open Jest;
open Expect;
open Belt.Result;

let (number, object_) = Js.Json.(number, object_);
let (decoder, succeed, map2, field, required, run) = Decode_Pipeline.(make, succeed, map2, field, required, run);

module Point = {
  type t = { x: float, y: float };
  let make = (x, y) => { x, y };
};

let pointJSON = Js.Dict.fromList([("x", number(3.1)), ("y", number(2.))]) |> object_;

describe("Test lazily executing decoders with a single JSON object", () => {
  let decoded =
    map2(
      Point.make,
      field("x", decoder(Decode.decodeFloat)),
      field("y", decoder(Decode.decodeFloat))
    ) |. run(pointJSON);

  test("Lazy execution successfully parses point", () => expect(decoded) |> toEqual(Ok(Point.make(3.1, 2.0))));
});

describe("Test piping to build up decoders, using |>", () => {
  let decoded =
    succeed(Point.make)
      |> required("x", decoder(Decode.decodeFloat))
      |> required("y", decoder(Decode.decodeFloat))
      |. run(pointJSON);

  test("Pipeline of required fields parses point", () => expect(decoded) |> toEqual(Ok(Point.make(3.1, 2.0))));
});
