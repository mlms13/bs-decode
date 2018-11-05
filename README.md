# bs-decode

Decode JSON values into structured ReasonML and OCaml types. Inspired by Elm's [Json.Decode](https://package.elm-lang.org/packages/elm-lang/core/5.1.1/Json-Decode) and the [Decode Pipeline](https://package.elm-lang.org/packages/NoRedInk/elm-decode-pipeline/3.0.1/Json-Decode-Pipeline), `bs-decode` is an alternative to [bs-json](https://github.com/glennsl/bs-json) that focuses on structured, type-safe error handling, rather than throwing exceptions.

## Installation

**Install via npm:**

`npm install --save bs-decode`

**Update your bsconfig.json**

```
"bs-dependencies": [
  "bs-decode"
],
```

## Why bs-decode?

ReasonML already has a de-facto standard library for JSON parsing in `bs-json`, but it throws exceptions when parsing is unsuccessful. Exceptions are hard to work with (because the compiler can't ensure you've checked exhaustively for them) and are largely unnecessary in a language that supports types like Belt's Result.

`bs-decode` offers many of the same tools for decoding JSON, but your values are ultimately wrapped in either an option or a result (or your own custom error type... more on that in a bit). As a bonus, you can collect all failures (instead of exiting on the first). This makes debugging much easier because you can log _everything_ that went wrong in a structured way, even for large, complex JSON objects.

## Usage

The `Decode` module gives you access to submodules that can decode into options or different kinds of results. They're all built on top of the same underlying tools, so the API doesn't differ much (only the return type). For the rest of this section, we'll decode into a `Belt.Result.t('a, Decode.ParseError)`. To make the code a little less cluttered, we'll assume you've added `open Decode.ParseError` at the top of the file and aliased the decode module as `module D = Decode.AsResult.OfParseError`.

**Decoding Simple Values**

You can parse simple (non-object, non-array) JSON values using `D.string`, `D.float`, and `D.int` (which will fail for numbers with floating point values... if you don't want this behavor, use `D.float` and map through `int_of_float` instead):

```reason
D.string(Js.Json.string("foo"));
/* Belt.Result.Ok("foo") */

D.string(Js.Json.number(3.14));
/* Error(Val(`ExpectedString, number(3.14))) */
```

If we had instead aliased `D` as `Decode.AsOption`, the first example would have yielded `Some("foo")` and the second would have been `None`.

**Decoding Arrays and Lists**

JSON arrays can be decoded into either arrays or lists. A decoder for the inner value is passed to the decode function. When using `Decode.ParseError`, inner decode failures will contain information about the position of the failure.

```reason
open Js;
let jsonArr = Json.array([| Json.string("a") |]);

D.decodeArray(D.string, jsonArr);
/* Ok([| "a" |]) */

D.decodeList(D.string, jsonArr);
/* Ok([ "a" ]) */

D.decodeList(D.int, jsonArr);
/* Error(Arr(NonEmptyList.pure((0, Val(`ExpectedInt, Json.string("a")))))) */
```

**Decoding Object Fields**

Decoding values from a JSON object requires specifying the string key of the field to be decoded, as well as an inner decode function to parse the value.

```reason
/* imagine `json` is `{ "foo": "bar", "baz": 4 }` */

D.field("foo", D.string, json);
/* Ok("bar") */

D.field("bar", D.int, json);
/* Ok(4) */

D.field("missing", D.int, json);
/* Error(Obj(NonEmptyList.pure(("missing", MissingField)))) */
```

If you need to dig through multiple nested objects, you can use `decodeAt`, which accepts a list of string field names as its first argument, instead of a single string.

After decoding the fields of an object, you'll probably want to combine it back into a ReasonML record. You can combine the results of multiple decoders using either the Haskell-style infix `map` and `apply` functions (`<$>` and `<*>`) or by building up a larger decoder in the style of Elm Decode Pipeline.

```reason
module User = {
  type t = { name: string, age: int };
  let make = (name, age) => { name, age };
};

/* Haskell style */
let ((<$>), (<*>)) = D.ResultUtil.Infix.((<$>), (<*>));
let decodeUser = json =>
  User.make
    <$> D.field("name", D.string, json)
    <*> D.field("age", D.int, json);

/* Pipeline style */
let decodeUser = json =>
  D.Pipeline.succeed(User.make)
    |> D.Pipeline.field("name", D.string)
    |> D.Pipeline.field("age", D.int)
    |> D.Pipeline.run(json);
```

Note that unlike previous decoders we've looked at, the pipeline style is not eager. Instead, it builds up a function that needs to be `run` with the JSON.

**Decoding Optional Values**

Wrapping a decode function in `D.optional` will convert the resulting type into an `option`, and will tolerate JSON null values.

```reason
optional(string, Js.Json.null); /* Ok(None) */
```

Similarly, for fields, you can use `D.optionalField` instead of `D.field` to tolerate both empty values as well as missing fields.

**It's important to note** that unlike Elm's decoding and `bs-json`, using `optional` or `optionalField` won't convert invalid data into `None`. For example, `D.optional(D.string, Js.Json.number(3.1))` will result in `Error(...)` instead of `Ok(None)`.

## Result? Option? NonEmptyList of String?

Out of the box, we provide decoders that will return the output of decoding as either an `option` or a `Belt.Result.t`.

  - `Decode.AsOption`: all functions return `Some(value)` for success or `None` for failures
  - `Decode.AsResult.OfParseError`: all functions return `Ok(value)` for success, or `Error(Decode.ParseError.t)` for failures
  - `Decode.AsResult.OfStringNel`: all functions return `Ok(value)` for success, or `Error(NonEmptyList.t(string))` for failures

Each of these has some pros and cons to consider:

**Option**

Pros:

- Lots of good tooling already exists to work with options
- You can add on your own layers of validation that also return options without worrying about unifying the errors

Cons:

- You lose all of the structural error information, even if all you want to do is log it

**Result of ParseError**

Pros:

- Errors are structured and can be checked exhaustively
- Great debugging information when you log the errors

Cons:

- Adding your own layer of validation comes with some boilerplate (see the Decode_Result_Custom_test.re test file for an example)

**Result of NonEmptyList String**

Pros:

- Reports all failures, giving decent debugging info
- Easy to extend with your own validation, as long as your validation returns a `Result.t('a, NonEmptyList.t(string))`

Cons:

- Strings are fine for logging, but bad for pattern matching

**The Winner?**

`Decode.AsResult.OfParseError` requires a bit more work up-front if you want to add your own custom validations, but the extra structure is probably worth it for anything larger than a trivial example project. `Decode.AsOption` is an easy way to get started, though, and switching to a `Result` decoder later should be trivial if you decide that's what you want.

## Contributing

1. Fork and clone this repository
2. `npm install`
3. Add features and tests
4. `npm run test`

A note on project structure:

- `DecodeBase.re` is where most of the actual functionality lives
- `Decode_As*.re` define the actual implementations, but those are minimal
- `Decode_ParseError.re` defines the error type useful for Results, as well as a collection of helpers to work with Results of that kind.

## License

Released under the MIT license. See `LICENSE`.
