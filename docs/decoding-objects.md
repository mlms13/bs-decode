---
id: decoding-objects
title: Decoding Object Fields
---

## Decoding Fields

Decoding values from a JSON object requires specifying the string key of the field to be decoded, as well as an inner decode function to parse the value.

```reason
/* imagine `obj` is `{ "foo": "xyz", "bar": 4 }` */

D.field("foo", D.string, obj);
/* Ok("xyz") */

D.field("bar", D.int, obj);
/* Ok(4) */

D.field("missing", D.int, obj);
/* Error(Obj(NonEmptyList.pure(("missing", MissingField)))) */

D.optionalField("missing", D.int, obj);
/* Ok(None) */
```

### Decoding Nested Fields

If a value you need is nested deeply in a JSON structure and you don't want to decode all of the intermediate bits of JSON, you can use `at` to dig through multiple keys:

```reason
/* json looks like { "a": { "b": { "c": false }}} */
D.at(["a", "b", "c"], D.boolean); /* Ok(false) */
D.at(["a", "b", "c", "d"], D.boolean); /* Error(Obj(("d", MissingField))) */
```

## Building Records

Back to that `user` type we defined in our [simple example](simple-example.md):

```reason
type user = {
  name: string,
  age: int,
  isAdmin: bool,
  lastLogin: option(Js.Date.t)
};

let make = (name, age, isAdmin, lastLogin) =>
  { name, age, isAdmin, lastLogin };
```

...the real goal of `bs-decode` is to give you the tools you need to build up complex record types (like this one) from JSON. You can certainly use `field` and `optionalField` to piece these things together, but two approaches follow that will make your life easier.

### Pipeline-Style

Given the `user` type above and its `make` function, you can build up a record by decoding each field in a style inspired by the [Elm Decode Pipeline](https://package.elm-lang.org/packages/NoRedInk/elm-decode-pipeline/3.0.1/) library for Elm. Order matters here, as each decoded field is passed into the next available slot in the `make` function.

```reason
let decode = json =>
  D.Pipeline.(
    succeed(make)
    |> field("name", D.string)
    |> field("age", D.intFromNumber)
    |> field("isAdmin", D.boolean)
    |> optionalField("lastLogin", D.date)
    |> run(json)
  );
```

Unlike other decode functions we've looked at, the Pipeline style is not eager. Instead, nothing will be decoded until the whole pipeline is executed by using the `run` function with the appropriate JSON.

### Haskell Validation-Style

It's also possible to use `map` and `apply` functions (often in their infix form `<$>` and `<*>`) to combine smaller decoders, using them to call the `make` function. This style my look more familiar if you've used validation libraries in Haskell.

```reason
let ((<$>), (<*>)) = D.ResultUtil.Applicative(map, apply);

let decode = json =>
  User.make
    <$> D.field("name", D.string, json)
    <*> D.field("age", D.intFromNumber, json)
    <*> D.field("isAdmin", D.boolean, json)
    <*> D.optionalField("lastLogin", D.date, json);
```
