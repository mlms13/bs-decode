---
id: what-and-why
title: What & Why
---

`bs-decode` gives developers in the Bucklescript world a way to decode JSON values into structured ReasonML and OCaml types. While popular alternatives already exist in the ecosystem, `bs-decode` differentiates itself by tracking **all** decoding failures (rather than failing on the first) and by avoiding runtime exceptions as a means of surfacing errors.

`bs-decode` was heavily influenced by Elm's [Json.Decode](https://package.elm-lang.org/packages/elm-lang/core/5.1.1/Json-Decode) and [Decode Pipeline](https://package.elm-lang.org/packages/NoRedInk/elm-decode-pipeline/3.0.1/Json-Decode-Pipeline) libraries.

## Exception-free

Exceptions are hard to work with. The compiler doesn't ensure that you've checked exhaustively for them, and they can't be stored as values or passed to functions once raised.

Instead, `bs-decode` chooses to represent errors as data, in the form of `option` or Belt's Result type. By representing decode failures as data, the compiler can help ensure that you've handled failures before accessing the data, leading to more resilient programs and better user experiences.

## Complete Errors

When decoding large objects or arrays of JSON data, you might encounter multiple failures. By collecting each failure instead of stopping once the first failure is reached, `bs-decode` gives you much more complete information for debugging. See [Working With Errors](working-with-errors.md) for examples of this.

Additionally, having access to a complete, structured representation of errors could allow for creative recovery or representation of those failures.
