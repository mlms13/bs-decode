
/* RESULT HELPER FUNCTIONS */
let fromOption = (failure, opt) => switch opt {
| None => Belt.Result.Error(failure)
| Some(v) => Belt.Result.Ok(v)
};

let alt = (a, b) => switch a {
| Belt.Result.Error(_) => b
| Belt.Result.Ok(v) => Belt.Result.Ok(v)
};

let map = (f, v) => switch v {
| Belt.Result.Error(x) => Belt.Result.Error(x)
| Belt.Result.Ok(v) => Belt.Result.Ok(f(v))
};

let pure = v => Belt_Result.Ok(v);

let optional = (decode, json) =>
  decode(json) |> map(v => Some(v)) |> alt(_, pure(None));

let decodePrim = (decode, primFailure, json) =>
  decode(json) |> fromOption(DecodeError.Primitive(primFailure, json));

let decodeString =
  decodePrim(Js.Json.decodeString, DecodeError.ExpectedString);

let decodeFloat =
  decodePrim(Js.Json.decodeNumber, DecodeError.ExpectedNumber);

let decodeInt = json =>
  decodeFloat(json) |. Belt.Result.map(int_of_float);

let decodeArray =
  decodePrim(Js.Json.decodeArray, DecodeError.ExpectedArray);