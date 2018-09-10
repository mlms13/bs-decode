type t = [
  | `ExpectedString(Js.Json.t)
  | `ExpectedNumber(Js.Json.t)
  | `ExpectedInt(Js.Json.t)
  | `ExpectedArray(Js.Json.t)
  | `ExpectedObject(Js.Json.t)
  | `InvalidArray(NonEmptyList.t((int, t)))
  | `InvalidObject(NonEmptyList.t((string, objError)))
]
and objError = [
  | `MissingField
  | `InvalidField(t)
  ];

let objPure = (name: string, err) =>
  `InvalidObject(NonEmptyList.pure((name, err)));

/**
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) => switch (a, b) {
| (`InvalidArray(nela), `InvalidArray(nelb)) => `InvalidArray(NonEmptyList.append(nela, nelb))
| (`InvalidObject(nela), `InvalidObject(nelb)) => `InvalidObject(NonEmptyList.append(nela, nelb))
| _ => a
};

/* let showValueError(err, json) = switch err {
| `ExpectedString => "Expected string"
| `ExpectedNumber => "Expected float"
| `ExpectedInt => "Expected int"
| `ExpectedArray => "Expected array"
| `ExpectedObject => "Expected object"
} ++ " but found: " ++ Js.Json.stringify(json); */

/* let rec toStringNel(err) = switch err {
| Primitive(v, json) => NonEmptyList.pure(showPrimError(v, json))
| Arr(nel) => NonEmptyList.flatMap(((idx, v)) => "At array position " ++ string_of_int(idx) ++ ": " ++ toStringNel(v), nel)
| Obj(nel) => NonEmptyList.flatMap(((field, v)) => "Parsing field " ++ field ++ ": " ++ showObjError(v), nel)
}
and showObjError(err) = switch err {
| MissingField => "Not found"
| ParseField(v) => toStringNel(v)
}; */
