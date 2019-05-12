open Relude.Globals;

type failure = [
  | `ExpectedBoolean
  | `ExpectedString
  | `ExpectedNumber
  | `ExpectedInt
  | `ExpectedArray
  | `ExpectedObject
  | `ExpectedValidDate
  | `ExpectedValidOption
];

let failureToPartialString =
  fun
  | `ExpectedBoolean => "Expected boolean"
  | `ExpectedString => "Expected string"
  | `ExpectedNumber => "Expected number"
  | `ExpectedInt => "Expected int"
  | `ExpectedArray => "Expected array"
  | `ExpectedObject => "Expected object"
  | `ExpectedValidDate => "Expected a valid date"
  | `ExpectedValidOption => "Expected a valid option";

let failureToString = (v, json) =>
  failureToPartialString(v) ++ " but found " ++ Js.Json.stringify(json);

module type TransformError = {
  type t('a);
  let valErr: (failure, Js.Json.t) => t('a);
  let arrErr: (int, t('a)) => t('a);
  let missingFieldErr: string => t('a);
  let objErr: (string, t('a)) => t('a);
  let lazyAlt: (t('a), unit => t('a)) => t('a);
};

module DecodeBase =
       (
         T: TransformError,
         M: BsAbstract.Interface.MONAD with type t('a) = T.t('a),
         Alt: BsAbstract.Interface.ALT with type t('a) = T.t('a),
       ) => {
  module InnerExtras = Relude.Extensions.Monad.MonadExtensions(M);

  module Monad:
    BsAbstract.Interface.MONAD with type t('a) = Js.Json.t => M.t('a) = {
    type t('a) = Js.Json.t => M.t('a);
    let map = (f, decode) => decode >> M.map(f);
    let apply = (f, decode, json) => M.apply(f(json), decode(json));
    let pure = (v, _) => M.pure(v);
    let flat_map = (decode, f, json) =>
      decode(json) |> M.flat_map(_, a => f(a, json));
  };

  module MonadFunctions = Relude_Extensions_Monad.MonadExtensions(Monad);

  let map = MonadFunctions.map;
  let apply = MonadFunctions.apply;
  let map2 = MonadFunctions.lift2;
  let map3 = MonadFunctions.lift3;
  let map4 = MonadFunctions.lift4;
  let map5 = MonadFunctions.lift5;
  let pure = MonadFunctions.pure;
  let flatMap = MonadFunctions.flatMap;

  let value = (decode, failure, json) =>
    decode(json) |> Option.foldLazy(() => T.valErr(failure, json), M.pure);

  let boolean = value(Js.Json.decodeBoolean, `ExpectedBoolean);

  let string = value(Js.Json.decodeString, `ExpectedString);

  let floatFromNumber = json =>
    value(Js.Json.decodeNumber, `ExpectedNumber, json);

  [@ocaml.deprecated "Use floatFromNumber instead."]
  let float = floatFromNumber;

  let intFromNumber = json => {
    let isInt = v => v == 0.0 || mod_float(v, floor(v)) == 0.0;
    floatFromNumber(json)
    |> M.flat_map(_, v =>
         isInt(v) ? M.pure(int_of_float(v)) : T.valErr(`ExpectedInt, json)
       );
  };

  [@ocaml.deprecated "Use intFromNumber instead."]
  let int = intFromNumber;

  let date = json =>
    json
    |> floatFromNumber
    |> M.map(Js.Date.fromFloat)
    |> Alt.alt(_, M.map(Js.Date.fromString, string(json)))
    |> M.flat_map(_, result =>
         result
         |> Js.Date.toJSONUnsafe
         |> Js.Nullable.return
         |> Js.Nullable.isNullable
           ? T.valErr(`ExpectedValidDate, json) : M.pure(result)
       );

  let variantFromJson = (jsonToJs, jsToVariant, json) =>
    jsonToJs(json)
    |> M.map(jsToVariant)
    |> M.flat_map(
         _,
         Option.foldLazy(() => T.valErr(`ExpectedValidOption, json), M.pure),
       );

  let variantFromString = (stringToVariant, json) =>
    variantFromJson(string, stringToVariant, json);

  let variantFromInt = (intToVariant, json) =>
    variantFromJson(intFromNumber, intToVariant, json);

  let optional = (decode, json) =>
    switch (Js.Json.decodeNull(json)) {
    | Some(_) => M.pure(None)
    | None => decode(json) |> M.map(v => Some(v))
    };

  let array = (decode, json) => {
    let decodeEach = arr =>
      Array.foldLeft(
        ((pos, acc), curr) => {
          let decoded = T.arrErr(pos, decode(curr));
          let result = InnerExtras.lift2(flip(Array.append), acc, decoded);
          (pos + 1, result);
        },
        (0, M.pure([||])),
        arr,
      );

    value(Js.Json.decodeArray, `ExpectedArray, json)
    |> M.flat_map(_, decodeEach >> snd);
  };

  let list = (decode, json) => array(decode, json) |> M.map(Array.toList);

  let dict = (decode, json) => {
    let rec decodeEntries =
      fun
      | [] => M.pure([])
      | [(key, value), ...xs] =>
        InnerExtras.lift2(
          (decodedValue, rest) => [(key, decodedValue), ...rest],
          T.objErr(key, decode(value)),
          decodeEntries(xs),
        );

    value(Js.Json.decodeObject, `ExpectedObject, json)
    |> M.map(Js.Dict.entries >> Array.toList)
    |> M.flat_map(_, decodeEntries)
    |> M.map(Js.Dict.fromList);
  };

  let rec at = (fields, decode, json) =>
    switch (fields) {
    | [] => decode(json)
    | [x, ...xs] =>
      value(Js.Json.decodeObject, `ExpectedObject, json)
      |> M.map(Js.Dict.get(_, x))
      |> M.flat_map(_, Option.fold(T.missingFieldErr(x), M.pure))
      |> M.flat_map(_, v => T.objErr(x, at(xs, decode, v)))
    };

  let field = (name, decode, json) => at([name], decode, json);

  let optionalField = (name, decode, json) =>
    value(Js.Json.decodeObject, `ExpectedObject, json)
    |> M.map(Js.Dict.get(_, name))
    |> M.flat_map(_, opt =>
         switch (opt) {
         | None => M.pure(None)
         | Some(v) => optional(decode, v)
         }
       );

  let fallback = (name, decode, alt, json) =>
    Alt.alt(field(name, decode, json), M.pure(alt));

  let tuple = ((fieldA, decodeA), (fieldB, decodeB), json) =>
    InnerExtras.lift2(
      (a, b) => (a, b),
      field(fieldA, decodeA, json),
      field(fieldB, decodeB, json),
    );

  let oneOf = (decode, rest, json) =>
    List.foldLeft(
      (a, b) => T.lazyAlt(a, () => b(json)),
      decode(json),
      rest,
    );

  module Pipeline = {
    /**
     * `succeed` returns a `json => Result` decode function that ignores the `json`
     * argument and always returns `Ok`
     */
    let succeed = (v, _) => M.pure(v);

    let pipe = (a, b, json) => map2((|>), a, b, json);

    let field = (name, decode) => pipe(field(name, decode));

    let at = (fields, decode) => pipe(at(fields, decode));

    let optionalField = (name, decode) => pipe(optionalField(name, decode));

    let fallback = (name, decode, alt) => pipe(fallback(name, decode, alt));

    let hardcoded = v => pipe(succeed(v));

    /**
     * `run` takes a decoder and some json, and it passes that json to the
     * decoder. The result is that your decoder is run with the provided json
     */
    let run = (json, decode) => decode(json);
  };
};
