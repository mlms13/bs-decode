type primError =
  | ExpectedString
  | ExpectedNumber
  | ExpectedDate
  | ExpectedArray
  | ExpectedObject
  ;

type t =
  | Primitive(primError, Js.Json.t)
  ;