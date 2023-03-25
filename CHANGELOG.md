# Changelog

## 1.1.0 (Mar 25, 2023)

This release adds helpers to ease the transition away from the deprecated features in the 1.0 release. Barring any bugs that require fixing, this is the last planned release in the 1.x series.

### :warning: Deprecated features

- Some deprecations mentioned in the previous release notes weren't actually deprecated in the code. Those will now trigger compiler warnings.

### :sparkles: New features

- `hush` is a new function that takes a `Decode.AsResult.OfParseError` decoder and converts it into a `Js.Json.t => option('a)` decoder (effectively "hushing" the error). This should make the transition away from `Decode.AsOption` easier.
- `literalBool`, `literalTrue`, and `literalFalse` join the other `literal*` decoders that first decode, then further validate the output
- `intUnion` works like `stringUnion` and should make the transition away from `variantFromInt` easier

## 1.0.0 (Mar 9, 2023)

Version 1.0 is here! Very little has changed since 0.11.2; mostly this release indicates the stability (or lack of maintenance, depending on how you look at it) over the last several years. The 1.x releases will be the last to support BuckleScript as we turn our attention to Melange.

There are no breaking changes in this release, but there are a handful of deprecations. We should end up with decent alternatives to all of the features that will eventually be removed, but if any of this concerns you, please let me know!

### :warning: Deprecated features

  - `Decode.AsResult.OfStringNel` will be removed in an upcoming release. The errors are less useful than `OfParseError`, and there's a cost to maintaining multiple decoder types
  - `Decode.AsOption` will also be removed. Again, there's a maintenance burden in keeping it, and if you really want `option`, it's easy enough to convert the `result` return into an `option`.
  - `Decode.Make` (for making your own custom output types) will eventually be removed as we focus on `ParseError`
  - `variantFromJson` and `variantFromString` will be removed in favor of new, simpler, more consistent tools for decoding variants (see the new `literal` functions below)
  - The `Decode.Pipeline` module is now deprecated. See [the docs](https://mlms13.github.io/bs-decode/docs/decoding-objects) for alternatives and a teaser about upcoming changes that will make this experience way better
  - `stringMap` will be removed as we try to limit how much we depend on Belt-specific features. You can use the `dict` decoder instead and convert the `Js.Dict` to a `Belt.String.Map`.

### :sparkles: New features

  - Functions have been added to decode literal values e.g. `literalString`, `literalInt`, and `literalFloat`. These functions first decode to the expected type, then ensure the value exactly matches the provided value. This is useful for decoding unions of string literals (e.g. `"dev" | "prod"`)
  - Decoding sting unions is such a common need that we've provided a `stringUnion` function to make it even simpler

### :memo: Documentation

- The [documentation site](https://mlms13.github.io/bs-decode/docs/what-and-why) got a handful of updates to explain the deprecations
- The guide on [decoding variants](https://mlms13.github.io/bs-decode/docs/decoding-variants) was almost completely rewritten and might be particularly helpful

### :robot: Dependency updates

- `bs-bastet` was bumped to `2.0` and `relude` was bumped to `0.66.1`. As peer dependencies, this change is the most likely to impact your existing projects, but hopefully the update isn't too bad

## 0.11.2 (Jun 28, 2020)

### :bug: Bug fixes

- Compiled output files will no longer have a `/./` in the middle of the path, which causes issues with some bundlers/dev setups (thanks @hamza0867 for finding and fixing this!)

## 0.11.1 (Mar 31, 2020)

### :bug: Bug fixes

- Revert the `public` flag in the `bsconfig.json`. There are apparently some strange quirks that cause aliased modules not to compile in downstream projects, so I got rid of it. You still shouldn't need to access `Decode_*` modules directly (instead use `Decode.AsWhatever`) but those modules won't be hidden from you.

## 0.11.0 (Mar 31, 2020)

### :rotating_light: Breaking changes

- The `bs-abstract` peer-dependency is now `bs-bastet`, and the required Relude version is 0.59+. See the [Relude release notes](https://github.com/reazen/relude/releases/tag/v0.59.0) and the [Bastet migration guide](https://github.com/Risto-Stevcev/bastet/blob/1.2.5/MIGRATION.md) for details.
- Alias everything in `Decode` and prevent direct access to the `Decode_*` modules via Bucklescript's `public` flag

### :sparkles: New features

- `okJson` is a decoder that always passes and preserves the input JSON

## 0.10.0 (Mar 3, 2020)

### :rotating_light: Breaking changes

- All error-related types now live in `Decode.ParseError`. Specifically, this means that `DecodeBase.failure` is now `Decode.ParseError.base`, which is important if you're extending the base errors to create your own custom errors.

### :memo: Documentation

- README explains peer dependencies better
- Nested array decoding is demonstrated in the tests
- Haskell-style object decoding operates on the decoders, not the result

### :heavy_check_mark: Code quality

- Bump dependencies and allow compilation with Bucklescript 7.1
- Internally, use structural typing for typeclasses rather than named modules

## 0.9.0 (Oct 7, 2019)

### :sparkles: New features

- Decode into a Belt Map (with string keys)
- Allow easy access to tuple helpers from Pipeline
- `pipe` function itself is now public

### :heavy_check_mark: Code quality

- More tests around string and int decoding
- Bump dependencies

## 0.8.1 (Jul 12, 2019)

### :bug: Bug fixes

- `optionField` values that are present but fail to decode will now report the field in the `ParseError`

### :heavy_check_mark: Code quality

- CI will now show failures correctly if tests don't pass

## 0.8.0 (Jul 3, 2019)

### :rotating_light: Breaking

- Long-deprecated `int` and `float` functions have been removed. You should be using `intFromNumber` and `floatFromNumber` instead to avoid shadowing issues
- `tuple` (which previously contructed a `tuple` from a JSON object) is now `tupleFromFields`
- Base `failure` type now includes an `ExpectedTuple(int)` constructor, where `int` is the expected size. This is only a breaking change if you're manually matching on values of the base `failure` type.

### :sparkles: New features

- JSON arrays can now be decoded directly into tuples using `tuple2`...`tuple5` (which will fail if the JSON array is larger than expected) or `tupleAtLeast2`...`tupleAtLeast5` (which will tolerate longer arrays)

## 0.7.0 (Jun 25, 2019)

### :rotating_light: Breaking

- `relude` and `bs-abstract` are now peerDependencies. This means you'll need to add these dependencies to your own `package.json`, and you're much less likely to end up with duplicate versions of these packages.

## 0.6.2 (Jun 7, 2019)

### :bug: Bug fixes

- `Decode.array` (and its friend `list`) had a regression where they could fail in some browsers when given large amounts of data, but stack safety has been restored

## 0.6.1 (May 28, 2019)

### :heavy_check_mark: Code quality

- Fix links in `package.json`

## 0.6.0 (May 27, 2019)

### :rotating_light: Breaking

- `Decode.fallback` doesn't assume you want to work with `field`s, but you can use `Decode.(fallback(field("x", string), "default"))` if you want the old behavior
- `Decode.Pipeline.fallback` has the same new behavior, but it provides `fallbackField` to achieve the old behavior
- `Decode.ParseError.map` has been removed (it wasn't used internally or documented)
- The `Decode.ParseError` variant type now includes a `TriedMultiple` constructor. This is only a breaking change if you are matching directly on values of this type.

### :bug: Bug fixes

- `Decode.AsResult.OfStringNel` actually collects multiple errors now

### :sparkles: New features

- `Decode.alt` allows combining decode functions and picking the first success
- `Decode.AsResult.OfStringNel` now includes all of the same `map`, `flatMap`, etc functions for decoders
- `Decode.ParseError` is aliased as `Decode.AsResult.OfParseError.ParseError` so parse errors can be accessed from an aliased decode module

### :heavy_check_mark: Code quality

- Reorganize tests so that `Decode_AsOption` tests decoders pass-vs-fail, `Decode_AsResult_*` tests failure reporting
- Test coverage has increased to 100%
- Internally, many functions were re-written to use `map`, `flatMap`, `alt`, etc on the decoders themselves, rather than running the decoders and transforming the output
- `Js.Dict.key` was changed to `string` in interface files for better editor suggestions

## 0.5.1 (May 22, 2019)

### :heavy_check_mark: Code quality

- Bump Relude dependency to the latest version
- Add continuous integration and coverage reporting

## 0.5.0 (May 13, 2019)

### :rotating_light: Breaking

- `Decode.oneOf` now takes a single decoder, followed by a list of decoders instead of requiring you to construct a `NonEmptyList` (#28)
- `Decode.ok` has been removed; if you want to construct a decoder that always succeeds (ignoring the JSON input), use `Decode.pure` instead
- Decoders that produce `NonEmptyList`s of errors now use `Relude.NonEmpty.List` instead of the implementation from `bs-nonempty` due to more active maintenance and a broader collection of helper functions
- `Decode.ResultUtils` is no longer part of the public interface. Decode functions themselves are more easily composable (see "New stuff" below), so there's less need to transform the output after decoding. Plus [better libraries](https://github.com/reazen/relude) exist if you want to work with `option` and `result` types. (#33)

### :sparkles: New features

- Decode functions now have `map`, `apply`, `map2`...`map5`, and `flatMap` functions, meaning you can transform decoders before actually running them. (#23)
- `Decode.NonEmptyList` is exposed for all decode modules that return a `NonEmptyList` of errors. This means you can do basic operations with the errors without bringing in an additional library (#24)
- `Decode.Pipeline` aliases most of the base decoders (e.g. `string`, `intFromNumber`, etc), so locally-opening `Decode.Pipeline` should get you everything you need (#27)

### :heavy_check_mark: Code quality

- Internal use of infix functions has been greatly reduced to improve code clarity
- Tests have fewer global opens and fewer aliased functions

## 0.4.0 (Jan 24, 2019)

### :rotating_light: Breaking

- `float` and `int` are now `floatFromNumber` and `intFromNumber` to avoid compiler warnings related to shadowing `Pervasives.float`

### :sparkles: New features

- `Decode.date` decodes numbers or ISO-8601 strings into `Js.Date.t`
- `Decode.oneOf` attempts multiple decoders
- `Decode.dict` decodes an object into a `Js.Dict.t`
- `Decode.variantFromJson` (and `variantFromString`, `variantFromInt`) decode JSON values directly into Reason variants
- `Decode.Pipeline.at` now brings `at` functionality into the pipeline, to allow digging into nested JSON objects

### :memo: Documentation

- The `docs/` folder has more content
- The website is unpublished but mostly finished

## 0.3.3 (Oct 12, 2018)

### :bug: Bug fixes

- `Decode.int` now works correctly with `0`

## 0.3.1 (Oct 12, 2018)

### :sparkles: New features

- `Decode.AsResult.OfStringNel` now provides a consistent `ResultUtil` module that matches `...OfParseError`

### :heavy_check_mark: Code quality

- `*.rei` files now exist to help with editor suggestions


## 0.3.0 (Oct 10, 2018)

### :rotating_light: Breaking

- Rename `decode*` functions (e.g. `decodeString` is now `string`)
- Flip arguments to `Pipeline.run` so it can be piped with `|>` (thanks @gavacho)

### :sparkles: New features

- `Decode.boolean` was missing but now exists (thinks @johnhaley81)

## 0.2.0 (Sep 28, 2018)

### :rotating_light: Breaking

- Basic failure type is now a polymorphic variant so it's easier to extend with custom validations
- Many submodules were renamed and reorganized

### :sparkles: New features

- `Decode` top-level modules provides a convenient way to access everything

### :memo: Documentation

- Some basic examples now exist

## 0.1.0 (Sep 24, 2018)

Initial release includes basic decode functions and the choice of decoding into an `option` or a `Belt.Result.t` with recursively structured errors.
