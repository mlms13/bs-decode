---
id: working-with-errors
title: Working With Errors
---

## The Error Type

When using `Decode.AsResult.OfParseError`, each decoder can fail with a Result Error that holds a variant value representing the structured decode error.

The underlying/primitive decode errors look like:

```reasonml
type failure = [
  | `ExpectedBoolean
  | `ExpectedString
  | `ExpectedNumber
  | `ExpectedInt
  | `ExpectedArray
  | `ExpectedTuple(int) // expected size of
  | `ExpectedObject
  | `ExpectedValidDate
  | `ExpectedValidOption
]
```

The full error structure (where the base `Val` errors hold the failure type above) looks like:

```reasonml
type t =
  | Val(failure, Js.Json.t)
  | TriedMultiple(NonEmptyList.t(t))
  | Arr(NonEmptyList.t((int, t)))
  | Obj(NonEmptyList.t((string, objError)))
and objError =
  | MissingField
  | InvalidField(t);
```

Ultimately, this is saying that a `ParseError` will be either a `Val` error, a `TriedMultiple` error, an `Arr` error, or an `Obj` error.

- `Val` errors contain one of the primitive errors defined above and the JSON that it failed to decode
- `TriedMultiple` happens when multiple decoders were attempted (e.g. using `alt`) and it contains a non-empty list of other errors
- `Arr` errors hold a non-empty list of position/error pairs
- `Obj` errors hold a non-empty list of fieldname/object-error pairs
  - Object errors happen either because the field is a `MissingField`, or
  - the field exists but its value couldn't be decoded (`InvalidField` which recursively contains any of the above errors)

This error structure can be destructured with recursive pattern matching.

## Logging

One of the most common reasons to pattern match on the error structure is to log what went wrong while decoding. This is a common enough use case that helpers are provided to do just that:

```reasonml
switch (decode(json)) {
| Ok(v) => ... // do something with your successful value
| Error(err) =>
  Js.log(Decode.ParseError.failureToDebugString(err))
};
```

The actual output of `failureToDebugString` will depend on the decoder and the JSON value you pass to it, but the string output of that function could look something like:

```sh
Failed to decode array:
    At position 3: Failed to decode object:
        Field "foo" is required but was not present
        Field "bar" had an invalid value: Failed to decode array:
            At position 0: Expected int but found []
```
