---
id: return-types
title: Return Types
---

`bs-decode` provides functions that will return values wrapped in either an `option` or a `Belt.Result.t`. Many of these error handling ideas were inspired by [Composable Error Handling in OCaml](http://keleshev.com/composable-error-handling-in-ocaml), which is a great read that goes into more detail than our summary here.

  - `Decode.AsOption`: all functions return `Some(value)` for success or `None` for failures
  - `Decode.AsResult.OfParseError`: all functions return `Ok(value)` for success, or `Error(Decode.ParseError.t)` for failures
  - `Decode.AsResult.OfStringNel`: all functions return `Ok(value)` for success, or `Error(NonEmptyList.t(string))` for failures

Each of these has some pros and cons to consider:

## Option

Pros:

- Lots of good tooling already exists to work with options
- You can add on your own layers of validation that also return options without worrying about unifying the errors

Cons:

- You lose all of the structural error information, even if all you want to do is log it

## Result of ParseError

Pros:

- Errors are structured and can be checked exhaustively
- Great debugging information when you log the errors

Cons:

- Adding your own layer of validation comes with some boilerplate (see [Decoding Variants](decoding-variants.md))

## Result of NonEmptyList String

Pros:

- Reports all failures, giving decent debugging info
- Easy to extend with your own validation, as long as your validation returns a `Result.t('a, NonEmptyList.t(string))`

Cons:

- Strings are fine for logging, but bad for pattern matching

## The Winner?

`Decode.AsResult.OfParseError` requires [a bit more work up-front](decoding-variants.md) if you want to add your own custom validations, but the extra structure is probably worth it for anything larger than a trivial example project. `Decode.AsOption` is an easy way to get started, though, and switching to a `Result` decoder later should be trivial if you decide that's what you want.
