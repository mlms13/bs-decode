## Unreleased

The following changes are currently in the `master` branch but have not been published in a tagged release yet.

### :rotating_light: Breaking

- `Decode.fallback` doesn't assume you want to work with `field`s, but you can use `Decode.(fallback(field("x", string), "default"))` if you want the old behavior
- `Decode.Pipeline.fallback` has the same new behavior, but it provides `fallbackField` to achieve the old behavior
- `Decode.ParseError.map` has been removed (it wasn't used internally or documented)

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
