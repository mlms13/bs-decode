type primError =
  | ExpectedString
  | ExpectedNumber
  | ExpectedDate
  | ExpectedArray
  | ExpectedObject
  ;

type t =
  | Primitive(primError, Js.Json.t)
  | Arr(NonEmptyList.t((int, t)))
  | Obj(NonEmptyList.t((string, objError)))
  /* | Custom(string, Js.Json.t) */

  and objError =
  | MissingField
  | ParseField(t)
  ;

let objPure = (name, err) =>
  Obj(NonEmptyList.pure((name, err)));

/**
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) => switch (a, b) {
| (Arr(nela), Arr(nelb)) => Arr(NonEmptyList.append(nela, nelb))
| (Obj(nela), Obj(nelb)) => Obj(NonEmptyList.append(nela, nelb))
| _ => a
};
