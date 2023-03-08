---
id: return-types
title: Return Types
---

`bs-decode` provides decode functions that will return values wrapped in either an `option` or a `result`. Many of these error handling ideas were inspired by [Composable Error Handling in OCaml](http://keleshev.com/composable-error-handling-in-ocaml), which is a great read that goes into more detail than our summary here.

  - `Decode.AsResult.OfParseError`: all functions return `Ok(value)` for success, or `Error(Decode.ParseError.t)` for failures
  - `Decode.AsResult.OfStringNel`: all functions return `Ok(value)` for success, or `Error(NonEmptyList.t(string))` for failures
  - `Decode.AsOption`: all functions return `Some(value)` for success or `None` for failures

## Result of ParseError

While it's possible to choose other error representations when using `bs-decode`, this is the preferred choice for decoding, as it provides the richest error information. In a future release, this will be the only supported value for holding the decode output.

For more information on how to work with the structured error data returned by these decoders, see [Working With Errors](working-with-errors.md).

## Option (Deprecated)

If you prefer to work with `option` for the sake of simplicity, it's recommended that you still use `Decode.AsResult.OfParseError` and convert the `result` to `option` after running the decoder.

## Result of NonEmptyList String (Deprecated)

Instead of providing structured, typed error information when a decoder fails, `Decode.AsResult.OfStringNel` returns a non-empty list of `string` error messages. The benefit is that string error messages are easy to read and easy to extend with your own validation error messages. However, the error messages provided aren't as rich in information as the errors when using `Decode.AsResult.OfParseError`, and if your goal is simply to read the output, it's better to use those decoders and convert the error messages to a string using `Decode.ParseError.failureToDebugString`.

We're hoping to reduce the need for custom error messages (the biggest benefit of using this set of decoders), and maintaining multiple types of `result` decoders adds quite a bit of complexity to the `bs-decode` library, so this set of decoders will likely be removed in a future release.
