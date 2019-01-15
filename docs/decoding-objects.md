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
```

### Decoding Optional Fields

TODO

### Decoding Nested Fields

TODO

## Building Records

### Pipeline-Style

TODO

### Haskell Validation-Style

TODO
