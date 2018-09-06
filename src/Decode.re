
/* RESULT HELPER FUNCTIONS */
let (map, mapErr, alt) = ResultDecodeError.(map, mapErr, alt);
let fromOption = (failure, opt) => switch opt {
| None => Belt.Result.Error(failure)
| Some(v) => Belt.Result.Ok(v)
};

let pure = v => Belt_Result.Ok(v);

let flatMap = (f, v) => switch v {
| Belt.Result.Error(x) => Belt.Result.Error(x)
| Belt.Result.Ok(v) => f(v)
};

let recover = (decode, json) =>
  decode(json) |> map(v => Some(v)) |> alt(_, pure(None));

let decodePrim = (decode, primFailure, json) =>
  decode(json) |> fromOption(DecodeError.Primitive(primFailure, json));

let decodeString =
  decodePrim(Js.Json.decodeString, DecodeError.ExpectedString);

let decodeFloat =
  decodePrim(Js.Json.decodeNumber, DecodeError.ExpectedNumber);

let decodeInt = json =>
  decodeFloat(json) |. Belt.Result.map(int_of_float);

let decodeArray = (decode, json) => {
  let (>.) = BsAbstract.Function.Infix.(>.);

  let decodeEach = BsAbstract.Array.Foldable.fold_left(((pos, acc), curr) => {
    open Belt.Result;
    let result = switch (acc, decode(curr)) {
    | (Ok(arr), Ok(v)) => Ok(Array.append(arr, [|v|]))
    | (Ok(_), Error(x)) => Error(NonEmptyList.pure((pos, x)))
    | (Error(errs), Error(x)) => Error(NonEmptyList.cons((pos, x), errs))
    | (Error(errs), Ok(_)) => Error(errs)
    };
    (pos + 1, result)
  }, (0, pure([||]))) >. snd >. mapErr(NonEmptyList.reverse);

  decodePrim(Js.Json.decodeArray, DecodeError.ExpectedArray, json)
    |> map(decodeEach)
    |> flatMap(mapErr(x => DecodeError.Arr(x)));
};

let decodeList = (decode, json) =>
  decodeArray(decode, json) |> map(Array.to_list);

let decodeField = (name, decode, json) =>
  decodePrim(Js.Json.decodeObject, DecodeError.ExpectedObject, json)
    |> map(Js.Dict.get(_, name))
    |> flatMap(fromOption(DecodeError.objPure(name, DecodeError.MissingField)))
    |> flatMap(v => decode(v) |> mapErr(err => DecodeError.objPure(name, DecodeError.ParseField(err))));
