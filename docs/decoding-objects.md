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

### Let-Ops

With the switch to Melange, `bs-decode` now has access to newer OCaml features, such as [binding operators](https://v2.ocaml.org/manual/bindingops.html). This dramatically simplifies the syntax for decoding and constructing complex objects:

```reasonml
let decode = {
  open Decode;
  let+ name = field("name", string)
  and+ age = field("age", intFromNumber)
  and+ isAdmin = field("isAdmin", boolean)
  and+ lastLogin = optionalField("lastLogin", date);
  { name, age, isAdmin, lastLogin };
};
```

This is the preferred method of decoding JSON into Reason records, as it allows constructing tuples, records, and calling functions with labeled arguments.

### Haskell Validation Style

It's also possible to use `map` and `apply` functions (often in their infix form `<$>` and `<*>`) to build up a larger decoder from smaller ones. This style my look more familiar if you've used validation libraries in Haskell. Importantly, this will not work if `User.make` expects labeled arguments, and it's important to decode the fields in the same order that `User.make` expects to receive its arguments.

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
