# bs-decode

[![build status](https://img.shields.io/circleci/build/github/mlms13/bs-decode.svg?style=flat-square)](https://circleci.com/gh/mlms13/bs-decode)
[![test coverage](https://img.shields.io/coveralls/github/mlms13/bs-decode.svg?style=flat-square)](https://coveralls.io/github/mlms13/bs-decode)
[![npm version](https://img.shields.io/npm/v/bs-decode.svg?style=flat-square)](https://www.npmjs.com/package/bs-decode)
[![license](https://img.shields.io/github/license/mlms13/bs-decode.svg?style=flat-square)](https://github.com/mlms13/bs-decode/blob/master/LICENSE)

> **Note**
>
> bs-decode has been stable and used in production for several years, so a v1 release makes sense. This is the final release that will be compatible with BuckleScript as we turn our attention to the newer OCaml features available in Melange.

[Read the Documentation](https://mlms13.github.io/bs-decode/docs/)

Decode JSON values into structured ReasonML and OCaml types. Inspired by Elm's [Json.Decode](https://package.elm-lang.org/packages/elm-lang/core/5.1.1/Json-Decode) and the [Decode Pipeline](https://package.elm-lang.org/packages/NoRedInk/elm-decode-pipeline/3.0.1/Json-Decode-Pipeline), `bs-decode` is an alternative to [bs-json](https://github.com/glennsl/bs-json) that focuses on structured, type-safe error handling, rather than exceptions. Additionally, `bs-decode` collects up _everything_ that went wrong while parsing the JSON, rather than failing on the first error.

## Installation

**Install:**

`yarn add bs-decode relude bs-bastet`

**Add the library stanzas in your dune file**

If you're using [dune](https://dune.build/) to build your project, add a [library stanza](https://dune.readthedocs.io/en/stable/dune-files.html#library) to tell dune about `bs-decode`. You'll need to add similar library stanzas for `bsBastet` and `relude` if you haven't already.

```
(subdir
 node_modules/bs-decode/src
 (library
  (name bsDecode)
  (modes melange)
  (libraries bsBastet relude)))
```

## Usage

The following is available to give you an idea of how the library works, but [the complete documentation](https://mlms13.github.io/bs-decode/docs/simple-example) will probably be more useful if you want to write your own decoders.

```reason
// imagine you have a `user` type and `make` function to construct one
type user = {
  name: string,
  age: int,
  isAdmin: bool,
  lastLogin: option(Js.Date.t)
};

let make = (name, age, isAdmin, lastLogin) =>
  { name, age, isAdmin, lastLogin };

/**
 * Given a JSON value that looks like:
 * { "name": "Alice", "age": 44, "roles": ["admin"] }
 *
 * you can write a function to convert this JSON into a value of type `user`
 */
module Decode = Decode.AsResult.OfParseError; // module alias for brevity

let decode = json =>
  Decode.Pipeline.(
    succeed(make)
    |> field("name", string)
    |> field("age", intFromNumber)
    |> field("roles", list(string) |> map(List.contains("admin")))
    |> optionalField("lastLogin", date)
    |> run(json)
  );

let myUser = decode(json); /* Ok({ name: "Alice", ...}) */
```

## Contributing

All contributions are welcome! This obviously includes code changes and documentation improvements ([see CONTRIBUTING](https://github.com/mlms13/bs-decode/blob/master/CONTRIBUTING.md)), but we also appreciate any feedback you want to provide (in the form of [Github issues](https://github.com/mlms13/bs-decode/issues)) about concepts that are confusing or poorly explained in [the docs](https://mlms13.github.io/bs-decode/docs/what-and-why).

## License

Released under the [MIT license](https://github.com/mlms13/bs-decode/blob/master/LICENSE).
