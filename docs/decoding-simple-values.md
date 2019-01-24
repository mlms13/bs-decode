---
id: decoding-simple-values
title: Simple Values
---

The following decoders are provided out-of-the-box for decoding values from JSON. To keep things terse, we'll assume that `Decode.ParseError` and `Belt.Result` are open in the local scope and we've aliazed the decode module as `module D = Decode.AsResult.OfParseError`.

Note that the output of our decode functions is wrapped in a `Belt.Result.t` because we're using `Decode.AsResult....`. You could instead use `Decode.AsOption`, which would decode successes as `Some(value)` and errors as `None`.

**String**

```re
let json = Js.Json.string("foo");
D.string(json); /* Ok("foo") */

let json = Js.Json.number(3.14);
D.string(json); /* Error(Val(`ExpectedString, json)) */
```

**Int and Float**

JSON numbers can be decoded as either floats or ints. These decoders are suffixed with `...FromNumber` to allow you to open locally without getting compiler complaints about shadowing `float` from OCaml's Pervasives.

```re
let json = Js.Json.number(3.);
D.floatFromNumber(json); /* Ok(3.0) */
D.intFromNumber(json); /* Ok(3) */

let json = Js.Json.number(3.14);
D.floatFromNumber(json); /* Ok(3.14) */
D.intFromNumber(json); /* Error(`ExpectedInt, json) */
```

Note that `intFromNumber` will reject numbers with fractional parts (rather than rounding up or down). If you don't want this behavior, you can use `floatFromNumber`, then `map` the Result through `int_of_float`, which will drop the fractional part.

**Boolean**

```reason
let json = Js.Json.boolean(true);
D.boolean(json); /* Ok(true) */
```

**Lists and Arrays**

JSON arrays can be decoded into either arrays or lists. Decoding a JSON array requires you to also pass a decoder for the inner type.

```reason
let json = Js.Json.array([| Js.Json.string("a") |]);

D.array(D.string, json); /* Ok([| "a" |]) */
D.list(D.string, json); /* Ok([ "a" ]) */

/* Error(Arr(NonEmptyList.pure((0, Val(`ExpectedInt, Js.Json.string("")))))) */
D.list(D.intFromNumber, json);
```

**Date**

JSON doesn't natively support dates, but `bs-decode` provides decoders that will try to build a date from a JSON float or from a JSON string that can be understood by `new Date` in JavaScript.

```reason
let json = Js.Json.string("2018-11-17T05:40:35.869Z");
D.date(json); /* Ok(Js.Date.fromString("2018-11-17T05:40:35.869Z")) */

let json = Js.Json.number(1542433304450.0);
D.date(json); /* Ok(Js.Date.fromFloat(1542433304450.0));
```

While the `Js.Date.from*` functions in the comments above could return invalid Date objects, `bs-decode` will reject invalid dates as Result errors.

**Dict**

JSON objects can also be used to store key/value pairs, where the keys are strings. This can be represented as a `Js.Dict.t('a)` in ReasonML.

```reason
/**
 * Assume `json` is a value that looks like:
 * { "foo": 3, "bar": 2 }
 */

/* Ok(Js.Dict.fromList([("foo", 3), ("bar", 2)])) */
D.dict(D.intFromNumber, json);
```
