---
id: handling-errors
title: Handling Errors
---

## The Error Type

The following assumes you're working with `Decode.AsResult.OfParseError` (or [a customization](decoding-variants.md) built on top of it). We'll look at the type of the errors and how you can work with that type.

The underlying/primitive decode errors look like:

```re
type failure = [
  | `ExpectedBoolean
  | `ExpectedString
  | `ExpectedNumber
  | `ExpectedInt
  | `ExpectedArray
  | `ExpectedObject
  | `ExpectedValidDate
  | `ExpectedValidOption
]
```

You'll note that this is a polymorphic variant, allowing you to customize the underlying kinds of errors.

The full error type, used as the payload of the Result's `Error` constructor, looks like:

```re
type t =
  | Val(failure, Js.Json.t)
  | Arr(NonEmptyList.t((int, t)))
  | Obj(NonEmptyList.t((string, objError)))
and objError =
  | MissingField
  | InvalidField(t);
```

Ultimately, this is saying that a `ParseError` will be either a `Val` error, an `Arr` error, or an `Obj` error.

- `Val` errors contain one of the primitive errors defined above and the JSON that it failed to decode
- `Arr` errors hold a non-empty list of position/error pairs
- `Obj` errors hold a non-empty list of fieldname/ojbect-error pairs
  - Object errors happen either because the field is a `MissingField`, or
  - the field exists but its value couldn't be decoded (`InvalidField` which recursively contains any of the above errors)

This error structure can be destructured with recursive pattern matching.

## Logging

Pattern matching on the error structure with the intention of logging what went wrong is a common enough pattern that we provide utilities to help you with this.

```re
switch (runSomeDecoder(json)) {
| Belt.Result.Ok(v) => /* do something with your value */
| Belt.Result.Error(err) =>
  Js.log(Decode.ParseError.failureToDebugString(err))
};
```

Obviously, the output of `failureToDebugString` will depend on the decoder and the JSON value you pass to it, but the string output of that function could look something like:

```sh
Failed to decode array:
    At position 3: Failed to decode object:
        Field "foo" is required but was not present
        Field "bar" had an invalid value: Failed to decode array:
            At position 0: Expected int but found []
```

This same technique can be extended to your [custom decode errors](decoding-variants.md) as well.
