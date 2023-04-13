---
id: upgrading-to-v2
title: Upgrading To v2
---

In addition to modernizing the build, one of the main goals of `bs-decode` v2 is to remove features that are redundant or confusing, hopefully replacing those features with simpler alternatives.

All removed (or moved/renamed) features are documented below with a guide that should help you update to v2 from v1 (or a pre-1.0 release such as 0.11).

## Top-Level Modules

### Decode.AsResult.OfParseError

It's still there! This is now the only collection of decode functions, which can all be accessed via `Decode` directly.

### Decode.AsOption

This module has been removed. Instead, if you need to decode into an `option`, you can use the new `hush` function on any decoder, or you can use a function like Relude's `Result.toOption` after calling the decoder with JSON.

```reasonml
// before
let ((<$>), (<*>)) = Decode.AsOption(map, apply);
let decode = Decode.AsOption.(
  make
  <$> field("name", string)
  <*> field("age", intFromNumber)
);

// now, see also the note on decoding objects below
let decode = {
  open Decode;
  let+ name = field("name", string)
  and+ age = field("age", int)
}
|> Decode.hush;

// or, convert the Result to Option instead of using `hush`
let decodeAsOpt = json => decode(json) |> Relude.Result.toOption;
```

### Decode.AsResult.OfStringNel

This module has been removed. I doubt this will affect many projects, and there isn't a 1:1 replacement for it. The easiest option is to convert the `ParseError` to a `string` and construct a `Nel` with that string:

```reasonml
let decodeAsStringNel = json =>
  decode(json)
  |> Result.mapError(Decode.ParseError.failureToDebugString)
  |> Result.mapError(Relude.NonEmpty.List.singleton);
```

If this doesn't work for your needs, please open an issue, and I'm happy to help brainstorm a better migration plan.

## Combining Decoders, Decoding Records

With the addition of support for OCaml's binding operators, most of the previous ways of decoding records, tuples, and generally combining decoders with e.g. `map4` have been removed.

### Decode.Pipeline

`Pipeline`-style decoding has been removed. In addition to the benefit of having much less code to maintain internally, there are a couple benefits to users of `bs-decode`:

- The order in which you decode fields no longer matters
- You don't need to define a `make` constructor function for your record

```reasonml
// before
let decode = json =>
  Decode.Pipeline.(
    succeed((name, title, age) => {name, title, age})
    |> field("name", string)
    |> optionalField("title", string)
    |> field("age", intFromNumber)
    |> run(json)
  );

// now
let decode = {
  open Decode;
  let+ name = field("name", string)
  and+ title = optionalField("title", string)
  and+ age = field("age", int)
  { name, title, age };
};
```

### map2...map5

The `Decode.mapN` functions have been removed. If you were using functions like `map5` to decode 5 fields and construct a 5-field record, you can use the `let+`/`and+` strategy outlined above. If you were using `map2` to construct a tuple, you can use the new `pair` function:

```reasonml
// before
let decode =
  Decode.(
    map2(
      (name, birthday) => (name, birthday), // or e.g. Tuple2.make
      field("name", string),
      field("birthday", date),
    )
  );

// now
let decode =
  Decode.(
    pair(
      field("name", string),
      field("birthday", date),
    )
  )
```

### Decoding JSON Tuples

If you were previously using functions like `tuple2`...`tuple5` to decode JSON arrays of a fixed size (and potentially mixed types), you can now accomplish the same thing using the above `let+`/`and+` style in combination with a new `arrayAt` helper.

Note that this gets a bit more verbose, but it requires quite a bit less library code, and has a couple added benefits:

- It reuses the same concepts you already know from decoding records
- You can decode tuples of any size; you're no longer limited to 5-tuples

```reasonml
// json input of e.g. ["Alice", 32]
// before
let decode = Decode.(tuple2(string, intFromNumber));

// now
let decode = {
  open Decode;
  let+ first = arrayAt(0, string)
  and+ second = arrayAt(1, int);
  (first, second)
};
```

## Variants and Unions

### variantFromString, variantFromInt

The `variantFrom...` functions have been removed, but instead you can use `stringUnion` and `intUnion` to accomplish something similar:

```reasonml
// before
let decodeColor = Decode.variantFromString(
  fun
  | "blue" => Some(Blue)
  | "green" => Some(Green)
  | "red" => Some(Red)
  | _ => None
);

// now
let decodeColor = Decode.stringUnion(
  ("blue", Blue),
  [("green", Green), ("red", Red)]
);
```

Instead of a single generic `"Expected a valid option"` error, this will allow us to provide a more in-depth error message that explains each value that we were looking for and what we found instead.

## Custom Validations

Coming soon!

## Smaller Changes

### stringMap

We no longer offer the ability to decode directly into a `Belt.Map.String`, but we do still provide a `dict` decoder, and JS dictionaries can be easily converted to a `Belt.Map.String`:

```reasonml
// not sure why Belt doesn't provide this...
let beltMapFromDict = dict =>
  dict |> Js.Dict.entries |> Belt.Map.String.fromArray;

// the actual decoder
let decodeStringMap = innerDecode =>
  Decode.(dict(innerDecode) |> map(beltMapFromDict));
```
