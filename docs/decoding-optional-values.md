---
id: decoding-optional-values
title: Optional Values and Recovery
---

Sometimes, you may need to decode nullable JSON values, or JSON values that can be one of several different types. `bs-decode` provides several functions to help you in these cases.

**Optional Values**

Simple decoders can be wrapped in `D.optional` to allow them to tolerate `null` values and decode into `option('a)` rather than `'a`.

```reasonml
let jsonNull = Js.Json.null;
let jsonStr = Js.Json.string("foo");

Decode.(optional(string, jsonNull)); // Ok(None)
Decode.(optional(string, jsonStr)); // Ok(Some("foo"))
Decode.(optional(boolean, jsonStr)); // Error(Val(`ExpectedBoolean, jsonStr))
```

Note that unlike Elm's `Json.Decode`, optional values aren't atuomatically recovered as `None`. An optional `string` decoder will fail when given a JSON value that isn't a string.

**Optional Fields**

This gets into [decoding objects](decoding-objects.md), which is covered elsewhere. But it's important to be aware of the specialized `optionalField` function, in addition to the normal `optional` function. `optionalField` will tolerate both missing fields in a JSON object as well as present fields with `null` values, however like the normal `optional` function, this won't automatically recover from unexpected JSON.

```reasonml
let json: Js.Json.t = [%bs.raw {|
  {
    "name": "Michael",
    "age": null
  }
|}];

Decode.(optionalField("name", string, json)); // Ok(Some("Michael"))
Decode.(optionalField("age", intFromNumber, json)); // Ok(None)
Decode.(optionalField("isAdmin", boolean, json)); // Ok(None)
Decode.(optionalField("name", boolean, json)); // Error(...)

// compare with `optional` which is probably not what you want:

// Error(Val(`ExpectedInt, ...))
Decode.(optional(field("age", intFromNumber), json));

// Error(Obj(NonEmptyList.pure(("isAdmin", MissingField))))
Decode.(field("isAdmin", optional(boolean), json));
```

**Try Multiple Decoders**

If a JSON value could be one of several types, you can try multiple decoders in order using `alt` or `oneOf`. Decoding will end successfully on the first success, or with a `TriedMultiple` error once all provided decoders have been attempted.

Note that each attempt is evaluated lazily, so subsequent decoders will only be run if no success has been found yet.

```reasonml
// each decoder in `oneOf` has to return the same type
type t =
  | B(bool)
  | S(string)
  | I(int)
  | F(float);

let json = Js.Json.string("foo");

// functions from `json => Result.t(t, ...)`
let decodeB = Decode.(boolean |> map(v => B(v)));
let decodeS = Decode.(string |> p(v => S(v)));
let decodeI = Decode.(intFromNumber |> map(v => I(v)));
let decodeF = Decode.(floatFromNumber |> map(v => F(v)));

// here comes the part you actually care about

Decode.oneOf(decodeB, [decodeS, decodeI], json); // Ok(S("foo"))
Decode.oneOf(decodeB, [decodeI], json); // Error(Val(`ExpectedInt, ...))
```
