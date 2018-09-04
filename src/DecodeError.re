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
  ;

/**
 * TODO: define Result helpers (from bs-abstract) for `DecodeError`s
 */