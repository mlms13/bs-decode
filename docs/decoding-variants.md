---
id: decoding-variants
title: Decoding Variants
---

## Challenges With Variants

Many types in Reason have a 1-to-1 mapping to JSON value types (think strings, ints, floats, arrays, etc). This is not true of Reason's variant types. While it's certainly possible to store variant values in JSON, the exact representation will vary depending on the preferences of the person who encoded the value.

At a high level, variant types in Reason are a way to represent "or" relationships in the type system. When decoding JSON, `alt` (and the related `oneOf`) allows you to express that a JSON value may have one representation "or" a different representation. This forms the foundation of decoding variants. We'll look at more specific examples below.

### Simple Variants (Enums)

In their simplest form, Reason variant types are simply an enumeration of possible values, where each value carries no extra data. For example:

```reasonml
type color = Blue | Red | Green;
```

While there are a number of ways you could represent values of this type in JSON, let's assume that the values are encoded as the string values `"blue"` or `"red"` or `"green"`. In this case, you can write a decoder for each variant constructor and then combine them together using `alt` to form a single decoder:

```reasonml
let decodeBlue = Decode.(literalString("blue") |> map(_ => Blue));
let decodeRed = Decode.(literalString("red") |> map(_ => Red));
let decodeGreen = Decode.(literalString("green") |> map(_ => Green));

// decode by chaining together `alt`
let decode = Decode.(decodeBlue |> alt(decodeRed) |> alt(decodeGreen));

// the same decoder, using `oneOf`
let decode = Decode.oneOf(decodeBlue, [decodeRed, decodeGreen]);
```

The need to decode string literals from JSON into simple Reason variants is common enough that we provide a `stringUnion` decoder that allows you to simply express the relationship between strings and variant values as pairs:

```reasonml
let decode = Decode.stringUnion(
  ("blue", Blue),
  [("red", Red), ("green", Green)]
);
```

Whether you choose to use `alt`, `oneOf`, or `stringUnion`, you could expect the following outcomes when running your decode function:

```reasonml
// Ok(Blue)
decode(Js.Json.string("blue"));

// Error(Val(`ExpectedValidOption, ...))
decode(Js.Json.string("yellow"));
```

### Complex Variants

Not all variants are as simple as our `color` variant above. Often, each branch of a Reason variant could carry its own additional data. Consider the following:

```reasonml
type shape =
  | Rectangle(float, float)
  | Square(float)
  | Circle(float);
```

There are countless ways this data could be represented in JSON, but let's assume the following valid representations:

```json
[
  { "type": "rectangle", "width": 2.5, "height": 4 },
  { "type": "square", "side": 3 },
  { "type": "circle", "radius": 1.8 }
]
```

There's a little more to it now, but the same general strategy from before will still work. We can write a decoder for each possible variant branch, then we combine each decoder together using `oneOf`:

```reasonml
let decodeRectangle = Decode.(
  map3(
    (_, width, height) => Rectangle(width, height),
    field("type", literalString("rectangle")),
    field("width", floatFromNumber),
    field("height", floatFromNumber),
  )
);

let decodeSquare = Decode.(
  map2(
    (_, side) => Square(side),
    field("type", literalString("square")),
    field("side", floatFromNumber),
  )
);

let decodeCircle = Decode.(
  map2(
    (_, radius) => Circle(radius),
    field("type", literalString("circle")),
    field("radius", literalString("radius")),
  )
);

let decode = Decode.oneOf(decodeRectangle, [decodeSquare, decodeCircle]);
```

## Extending Base Errors (Deprecated)

**Note:** for complex variants, the above strategies involving `alt` already give you very detailed error information about each decoder that was attempted and failed. The downside of that approach is that `stringLiteral` decoders currently don't report back the specific string that the decoder was looking for. In the future, those errors will be improved, and the method of extending the error type itself (described below) will be removed.

In some cases, `ExpectedValidOption` might not be enough debugging information. You may want custom handling (or simply, more specific error messages) when something goes wrong during decoding.

You can get this by extending the underlying `Decode.ParseError.base` type with extra constructors. You use this extension to build your own custom `Decode.ParseError`, which in turn can be used to build a custom decode module on top of `Decode.Base`.

This may sound overwhelming, but the whole thing can be accomplished in about 6 lines of code:

```reasonml
module R =
  Decode.ParseError.ResultOf({
    type t = [ Decode.ParseError.base | `InvalidColor | `InvalidShape];
    let handle = x => (x :> t);
  });

module D = Decode.Base.Make(R.TransformError, R);
```

Now we have a `D` that is slightly different from the `Decode.AsResult.OfParseError` that we've been using up until now. This `D` can produce `Val` parse errors that can include `InvalidColor` or `InvalidShape` (in addition to the usual error cases).

We can change the `parseColor` function we defined above to return a Result where the Error is of type `InvalidColor` and it includes the specific piece of JSON we were trying to parse, for detailed debugging:

```reasonml
let parseColor =
  fun
  | "Blue" => Ok(Blue)
  | "Red" => Ok(Red)
  | "Green" => Ok(Green)
  | str => Error(Decode.ParseError.Val(`InvalidColor, Js.Json.string(str)));

let decodeColor = json => D.string(json) |> Result.flatMap(parseColor);
```

Now we have a `decodeColor` function that produces the same output type as any other `decode` function we may write, but it includes error information specific to our `color` parsing.
