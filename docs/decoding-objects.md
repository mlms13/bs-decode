---
id: decoding-objects
title: Decoding Object Fields
---

## Decoding Fields

Decoding values from a JSON object requires specifying the string key of the field to be decoded, as well as an inner decode function to parse the value.

```reasonml
// imagine `json` is `{ "foo": "xyz", "bar": 4 }`

// Ok("xyz")
Decode.field("foo", Decode.string, json);

// Ok(4)
Decode.field("bar", Decode.intFromNumber, json);

// Error(Obj(NonEmptyList.pure(("missing", MissingField))))
Decode.field("missing", Decode.intFromNumber, json);

// Ok(None)
Decode.optionalField("missing", Decode.intFromNumber, json);
```

### Decoding Nested Fields

If a value you need is nested deeply in a JSON structure and you don't want to decode all of the intermediate bits of JSON, you can use `at` to dig through multiple keys:

```reasonml
// json looks like { "a": { "b": { "c": false }}}
Decode.at(["a", "b", "c"], Decode.boolean); // Ok(false)

// Error(Obj(("d", MissingField)))
Decode.at(["a", "b", "c", "d"], Decode.boolean);
```

## Building Records

Back to that `user` type we defined in our [simple example](simple-example.md):

```reasonml
type user = {
  name: string,
  age: int,
  isAdmin: bool,
  lastLogin: option(Js.Date.t)
};

let make = (name, age, isAdmin, lastLogin) =>
  { name, age, isAdmin, lastLogin };
```

...the real goal of `bs-decode` is to give you the tools you need to build up complex record types (like this one) from JSON. You can certainly use `field` and `optionalField` to piece these things together, but the following approaches will make your life easier.

### Let-Ops (Coming Soon)

In the very near future, `bs-decode` will be switching from BuckleScript to Melange. This will give us access to newer OCaml features, such as [binding operators](https://v2.ocaml.org/manual/bindingops.html). This dramatically simplifies the syntax for decoding and constructing complex objects:

```reasonml
let decode =
  Decode.{
    let+ name = field("name", string)
    and+ age = field("age", intFromNumber)
    and+ isAdmin = field("isAdmin", boolean)
    and+ lastLogin = optionalField("lastLogin", date);
    User.make(name, age, isAdmin, lastLogin);
  };
```

Once available, this will replace the "Pipeline" decoding (see below). Unlike the other strategies outlined below, the order of the field decoders doesn't matter. It's much easier to see how each field is used in the constructor, and it works with labeled functions and literal record construction.

### Haskell Validation Style

It's also possible to use `map` and `apply` functions (often in their infix form `<$>` and `<*>`) to build up a larger decoder from smaller ones. This style my look more familiar if you've used validation libraries in Haskell.

```reasonml
let ((<$>), (<*>)) = Decode.(map, apply);

let decode =
  Decode.(
    User.make
    <$> field("name", string)
    <*> field("age", intFromNumber)
    <*> field("isAdmin", boolean)
    <*> optionalField("lastLogin", date)
  );
```

### Combining Decoders with `mapN`

The provided `map2`...`map5` functions can be used to take the results of up to 5 decoders and combine them using a function that receives the up-to-5 values if all decoders succeed. This is simple to use, but obviously limiting if the record you want to construct has more than 5 fields.

```reasonml
let decode =
  Decode.(
    map4(
      User.make,
      field("name", string),
      field("age", intFromNumber),
      field("isAdmin", boolean),
      optionalField("lastLogin", date),
    )
  );
```

### Pipeline-Style (Deprecated)

**Note:** This style of decoding has been deprecated and will be removed in v2.0.

Given the `user` type above and its `make` function, you can build up a record by decoding each field in a style inspired by the [Elm Decode Pipeline](https://package.elm-lang.org/packages/NoRedInk/elm-decode-pipeline/3.0.1/) library for Elm.

The order of decoded fields is significant, as the pipeline leverages the partial application of the `make` function. Each `field` or `optionalField` line in the example below fills in the next available slot in the `make` function.

```reasonml
let decode = json =>
  Decode.Pipeline.(
    succeed(make)
    |> field("name", string)
    |> field("age", intFromNumber)
    |> field("isAdmin", boolean)
    |> optionalField("lastLogin", date)
    |> run(json)
  );
```

Unlike other decode functions we've looked at, the Pipeline style is not eager. Instead, nothing will be decoded until the whole pipeline is executed by using the `run` function with the appropriate JSON.
