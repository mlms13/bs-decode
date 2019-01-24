---
id: decoding-variants
title: Decoding Variants
---

## Simple Variants

For variants that aren't very complex and don't require special debugging powers, you can use the built-in function `variantFromString` or `variantFromInt` to convert `string` and `int` values into values of your variant type. The conversion function can "fail" by returning `None`.

```reason
type color = Blue | Red | Green;
let parseColor = str =>
  switch (str) {
  | "Blue" => Some(Blue)
  | "Red" => Some(Red)
  | "Green" => Some(Green)
  | _ => None
  };

/* Ok(Blue) */
D.variantFromString(parseColor, Js.Json.string("Blue"));

/* Error(Val(`ExpectedValidOption, ...)) */
D.variantFromString(parseColor, Js.Json.string("Yellow"));
```

## Complex Variants

In other cases, `ExpectedValidOption` might not be enough debugging information. You may want custom handling (or simple more specific errors) when something goes wrong during decoding.

You can get this by extending the underlying `DecodeBase.failure` type with extra constructors. You use this extension to build your own custom `Decode.ParseError`, which in turn can be used to build a custom decode module on top of `DecodeBase`.

This may sound overwhelming, but the whole thing can be accomplished in about 6 lines of code:

```reason
module R =
  Decode.ParseError.ResultOf({
    type t = [ DecodeBase.failure | `InvalidColor | `InvalidShape];
    let handle = x => (x :> t);
  });

module D = DecodeBase.DecodeBase(R.TransformError, R.Monad, R.Alt);
```

Now we have a `D` that is slightly different from the `Decode.AsResult.OfParseError` that we've been using up until now. This `D` can produce `Val` parse errors that can be `InvalidColor` or `InvalidShape`.

We can change the `parseColor` function we defined above to return a Result where the Error is of type `InvalidColor` and it includes the specific piece of JSON we were trying to parse, for detailed debugging:

```reason
open Belt;

let parseColor = str =>
  switch (str) {
  | "Blue" => Result.Ok(Blue)
  | "Red" => Result.Ok(Red)
  | "Green" => Result.Ok(Green)
  | _ =>
    Result.Error(Decode.ParseError.Val(`InvalidColor, Js.Json.string(str)))
  };

let decodeColor = json =>
  D.string(json) -> Result.flatMap(parseColor);
```

Now we have a `decodeColor` function that produces the same output type as any other `decode` function we may write, but it includes error information specific to our `color` parsing.
