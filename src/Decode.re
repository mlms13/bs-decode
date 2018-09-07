/* RESULT HELPER FUNCTIONS */
let (map, mapErr, pure, flatMap, alt, fromOption) =
  ResultDecodeError.(map, mapErr, pure, flat_map, alt, note);

let decodePrim = (decode, primFailure, json) =>
  decode(json) |> fromOption(DecodeError.Primitive(primFailure, json));

let decodeString =
  decodePrim(Js.Json.decodeString, DecodeError.ExpectedString);

let decodeFloat =
  decodePrim(Js.Json.decodeNumber, DecodeError.ExpectedNumber);

let decodeInt = json => {
  let floatToResultInt = f => switch (mod_float(f, floor(f))) {
  | 0. => Belt.Result.Ok(int_of_float(f))
  | _ => Belt.Result.Error(DecodeError.Primitive(DecodeError.ExpectedInt, json))
  };
  decodeFloat(json) |> ResultDecodeError.flat_map(floatToResultInt)
};

let optional = (decode, json) =>
  switch (Js.Json.decodeNull(json)) {
  | Some(_) => pure(None)
  | None => decode(json) |> map(v => Some(v))
  };

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

let decodeFieldWithFallback = (name, decode, fallback, json) =>
  decodeField(name, decode, json) |> ResultDecodeError.recoverWith(fallback);

let decodeOptionalField = (name, decode, json) =>
  decodePrim(Js.Json.decodeObject, DecodeError.ExpectedObject, json)
    |> map(Js.Dict.get(_, name))
    |> flatMap(opt => switch opt {
      | None => pure(None)
      | Some(v) => optional(decode, v)
      });
