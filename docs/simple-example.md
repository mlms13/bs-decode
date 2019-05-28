---
id: simple-example
title: Simple Example
---

Let's explore the basics of decoding with a simple User record type:

```re
type user = {
  name: string,
  age: int,
  isAdmin: bool,
  lastLogin: option(Js.Date.t)
};
```

Next we'll build a decode function for our `user` type. `bs-decode` offers a few ways to decode JSON objects. For this example, we'll pick the "Pipeline" approach.

```re
// start with a simple constructor function for our type
let make = (name, age, isAdmin, lastLogin) =>
  { name, age, isAdmin, lastLogin };

// now we build up a decoder
let decode = json =>
  Decode.AsResult.OfParseError.(
    succeed(make)
    |> field("name", string)
    |> field("age", intFromNumber)
    |> field("isAdmin", boolean)
    |> optionalField("lastLogin", date)
    |> run(json)
  );
```

Finally, imagine we have the following JSON value:

```re
let obj: Js.Json.t = [%bs.raw {|
  {
    "name": "Michael",
    "age": 32,
    "isAdmin": true
  }
|}];
```

We can decode this JSON object:

```re
decode(obj); // Ok({ name: "Michael", ...})
```

Hopefully this is enough to get you started. You can learn more about [decoding primitive values](decoding-simple-values.md) and other techniques for [decoding objects](decoding-objects.md). If you want to [decode custom variants](decoding-variants.md) directly from the JSON, that's a bit more involved but definitely possible.

Many of the examples will locally open (or alias) `Decode.AsResult.OfParseError`. The full module path of that decoder is a mouthful, but [other options are available](return-types.md) if you'd rather decode into an Option or a Result with string errors.

Finally, we only demonstrated here what happens when you attempt to decode good JSON values. When you use the decoder we built above to decode bad JSON, you'll get back a `Belt.Result.Error` containing a structured error type. You can read more about [working with errors](working-with-errors.md).
