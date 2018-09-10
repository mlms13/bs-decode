/* RESULT HELPER FUNCTIONS */
let (map, map2, mapErr, pure, flatMap, alt, fromOption) =
  ResultDecodeError.(map, map2, mapErr, pure, flat_map, alt, note);

let decodeVal = (decode, valFailure, json) =>
  decode(json) |> fromOption(valFailure(json));

let decodeString =
  decodeVal(Js.Json.decodeString, v => `ExpectedString(v));

let decodeFloat =
  decodeVal(Js.Json.decodeNumber, v => `ExpectedNumber(v));

let decodeInt = json => {
  let isInt = v => mod_float(v, floor(v)) == 0.;
  decodeFloat(json) |> ResultDecodeError.flat_map(v =>
    if (isInt(v)) Belt.Result.Ok(int_of_float(v))
    else Belt.Result.Error(`ExpectedInt(json))
  )
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

  decodeVal(Js.Json.decodeArray, v => `ExpectedArray(v), json)
    |> map(decodeEach)
    |> flatMap(mapErr(x => `InvalidArray(x)));
};

let decodeList = (decode, json) =>
  decodeArray(decode, json) |> map(Array.to_list);

let rec decodeAt = (fields, decode, json) => switch fields {
| [] => decode(json)
| [x, ...xs] =>
  decodeVal(Js.Json.decodeObject, v => `ExpectedObject(v), json)
    |> map(Js.Dict.get(_, x))
    |> flatMap(fromOption(DecodeError.objPure(x, `MissingField)))
    |> flatMap(v => decodeAt(xs, decode, v) |> mapErr(err => DecodeError.objPure(x, `InvalidField(err))));
};

let decodeField = (name, decode, json) =>
  decodeAt([name], decode, json);

let decodeFieldWithFallback = (name, decode, fallback, json) =>
  decodeField(name, decode, json) |> ResultDecodeError.recoverWith(fallback);

let decodeOptionalField = (name, decode, json) =>
  decodeVal(Js.Json.decodeObject, v => `ExpectedObject(v), json)
    |> map(Js.Dict.get(_, name))
    |> flatMap(opt => switch opt {
      | None => pure(None)
      | Some(v) => optional(decode, v)
      });

let decodeTuple = ((fieldA, decodeA), (fieldB, decodeB), json) =>
  map2(
    (a, b) => (a, b),
    decodeField(fieldA, decodeA, json),
    decodeField(fieldB, decodeB, json)
  );
