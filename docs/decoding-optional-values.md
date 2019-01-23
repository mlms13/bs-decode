---
id: decoding-optional-values
title: Optional Values and Recovery
---

Sometimes, you may need to decode nullable JSON values, or JSON values that can be one of several different types. `bs-decode` provides several functions to help you in these cases.

**Optional Values**

Simple decoders can be wrapped in `D.optional` to allow them to tolerate `null` values and decode into `option('a)` rather than `'a`.

```re
let jsonNull = Js.Json.null;
let jsonStr = Js.Json.string("foo");

D.optional(D.string, jsonNull); /* Ok(None) */
D.optional(D.string, jsonStr); /* Ok(Some("foo")) */
D.optional(D.boolean, jsonStr); /* Error(Val(`ExpectedBoolean, jsonStr)) */
```

Note that unlike Elm's `Json.Decode`, optional values aren't atuomatically recovered as `None`. An optional `string` decoder will fail when given a JSON value that isn't a string.

**Optional Fields**

This gets into [decoding objects](decoding-objects.md), which is covered elsewhere. But it's important to be aware of the specialized `optionalField` function, in addition to the normal `optional` function. `optionalField` will tolerate both missing fields in a JSON object as well as present fields with `null` values, however like the normal `optional` function, this won't automatically recover from unexpected JSON.

```re
let json: Js.Json.t = [%bs.raw {|
  {
    "name": "Michael",
    "age": null
  }
|}];

D.optionalField("name", D.string, json); /* Ok(Some("Michael")) */
D.optionalField("age", D.intFromNumber, json); /* Ok(None) */
D.optionalField("isAdmin", D.boolean, json); /* Ok(None) */
D.optionalField("name", D.boolean, json); /* Error(...) */

/* compare with `optional` which is probably not what you want: */

/* Error(Val(`ExpectedInt, ...)) */
D.optional(D.field("age", D.intFromNumber), json);

/* Error(Obj(NonEmptyList.pure(("isAdmin", MissingField)))) */
D.field("isAdmin", D.optional(D.boolean), json);
```

**Try Multiple Decoders**

If a JSON value could be one of several types, you can try multiple decoders in order using `oneOf`. Decoding will end successfully on the first success, or with an error once all provided decoders have been tried.

Note that for now, `oneOf` expects a NonEmptyList of decoders. This can be a pain to construct, so the API is likely to change in a future version.

```re
module Nel = NonEmptyList; /* from bs-nonempty */

/* each decoder in `oneOf` has to return the same type */
type t =
  | B(bool)
  | S(string)
  | I(int)
  | F(float);

let json = Js.Json.string("foo");

let decodeB = json => D.boolean(json) -> Belt.Result.map(v => B(v));
let decodeS = json => D.string(json) -> Belt.Result.map(v => S(v));
let decodeI = json => D.intFromNumber(json) -> Belt.Result.map(v => I(v));
let decodeF = json => D.floatFromNumber(json) -> Belt.Result.map(v => F(v));

/* here comes the part you actually care about */

D.oneOf(Nel.make(decodeB, [decodeS, decodeI]), json); /* Ok(S("foo")) */
D.oneOf(Nel.make(decodeB, [decodeI]), json); /* Error(Val(`ExpectedInt, ...)) */
```
