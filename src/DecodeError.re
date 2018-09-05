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

  and objError =
  | MissingField
  | ParseField(t)
  ;

let objPure = (name, err) =>
  Obj(NonEmptyList.pure((name, err)));
/**
 * TODO: define Result helpers (from bs-abstract) for `DecodeError`s
 */