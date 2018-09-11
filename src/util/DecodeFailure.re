type valueErr =
  | ExpectedString
  | ExpectedNumber
  | ExpectedInt
  | ExpectedArray
  | ExpectedObject
  ;

type t =
  | Val(valueErr, Js.Json.t)
  | Arr(NonEmptyList.t((int, t)))
  | Obj(NonEmptyList.t((string, objError)))

and objError =
  | MissingField
  | InvalidField(t);


let objPure = (field, err) =>
  Obj(NonEmptyList.pure((field, err)));

/*
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) => switch (a, b) {
| (Arr(nela), Arr(nelb)) => Arr(NonEmptyList.append(nela, nelb))
| (Obj(nela), Obj(nelb)) => Obj(NonEmptyList.append(nela, nelb))
| _ => a
};
