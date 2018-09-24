type failure = [
  | `ExpectedString
  | `ExpectedNumber
  | `ExpectedInt
  | `ExpectedArray
  | `ExpectedObject
];

let failureToString = (v, json) => switch v {
| `ExpectedString => "Expected string"
| `ExpectedNumber => "Expected number"
| `ExpectedInt => "Expected int"
| `ExpectedArray => "Expected array"
| `ExpectedObject => "Expected object"
} ++ " but found " ++ Js.Json.stringify(json);

module type TransformError = {
  type t('a);
  let valErr: (failure, Js.Json.t) => t('a);
  let arrErr: (int, t('a)) => t('a);
  let missingFieldErr: string => t('a);
  let objErr: (string, t('a)) => t('a);
};

module DecodeBase = (
  T: TransformError,
  M: BsAbstract.Interface.MONAD with type t('a) = T.t('a),
  Alt: BsAbstract.Interface.ALT with type t('a) = T.t('a)
) => {
  module InfixMonad = BsAbstract.Infix.Monad(M);
  module InfixAlt = BsAbstract.Infix.Alt(Alt);

  let ((<$>), (<*>), (>>=)) = InfixMonad.((<$>), (<*>), (>>=));
  let ((<|>)) = InfixAlt.((<|>));
  let (>.) = BsAbstract.Function.Infix.(>.);

  let ok = v => M.pure(v);
  let map2 = (f, a, b) => f <$> a <*> b;
  let fromOpt = (fn, default, opt) =>
    BsAbstract.Option.maybe(~f=fn, ~default, opt);

  let decodeVal = (decode, failure, json) =>
    decode(json) |> fromOpt(ok, T.valErr(failure, json));

  let decodeString =
    decodeVal(Js.Json.decodeString, `ExpectedString);

  let decodeFloat = json =>
    decodeVal(Js.Json.decodeNumber, `ExpectedNumber, json);

  let decodeInt = json => {
    let isInt = v => mod_float(v, floor(v)) == 0.;
    decodeFloat(json) |. M.flat_map(v =>
      if (isInt(v)) ok(int_of_float(v))
      else T.valErr(`ExpectedInt, json)
    );
  };

  let decodeOpt = (decode, json) =>
    switch (Js.Json.decodeNull(json)) {
    | Some(_) => ok(None)
    | None => decode(json) |> M.map(v => Some(v))
    };

  let decodeArray = (decode, json) => {
    let decodeEach = arr => BsAbstract.Array.Foldable.fold_left(((pos, acc), curr) => {
      let decoded = T.arrErr(pos, decode(curr));
      let result = map2((arr, v) => Array.append(arr, [|v|]), acc, decoded);
      (pos + 1, result);
    }, (0, ok([||])), arr) |> snd;

    decodeVal(Js.Json.decodeArray, `ExpectedArray, json)
      |. M.flat_map(decodeEach);
  };

  let decodeList = (decode, json) =>
    decodeArray(decode, json) |> M.map(Array.to_list);

  let rec decodeAt = (fields, decode, json) => switch fields {
  | [] => decode(json)
  | [x, ...xs] =>
    decodeVal(Js.Json.decodeObject, `ExpectedObject, json)
      |> M.map(Js.Dict.get(_, x))
      |. M.flat_map(fromOpt(ok, T.missingFieldErr(x)))
      |. M.flat_map(v => T.objErr(x, decodeAt(xs, decode, v)));
  };

  let decodeField = (name, decode, json) =>
    decodeAt([name], decode, json);

  let decodeFieldWithFallback = (name, decode, fallback, json) =>
    decodeField(name, decode, json) <|> M.pure(fallback);

  let decodeOptionalField = (name, decode, json) =>
    decodeVal(Js.Json.decodeObject, `ExpectedObject, json)
      |> M.map(Js.Dict.get(_, name))
      |. M.flat_map(opt => switch opt {
        | None => ok(None)
        | Some(v) => decodeOpt(decode, v)
        });

  let decodeTuple = ((fieldA, decodeA), (fieldB, decodeB), json) =>
    map2(
      (a, b) => (a, b),
      decodeField(fieldA, decodeA, json),
      decodeField(fieldB, decodeB, json)
    );

  module Pipeline = {
    /**
     * `succeed` returns a `json => Result` decode function that ignores the `json`
     * argument and always returns `Ok`
     */
    let succeed = v => _ => ok(v);

    let map2 = (f, a, b) =>
      json => f <$> a(json) <*> b(json);

    let map3 = (f, a, b, c) =>
      json => f <$> a(json) <*> b(json) <*> c(json);

    let map4 = (f, a, b, c, d) =>
      json => f <$> a(json) <*> b(json) <*> c(json) <*> d(json);

    let map5 = (f, a, b, c, d, e) =>
      json => f <$> a(json) <*> b(json) <*> c(json) <*> d(json) <*> e(json);

    let pipe = (a, b, json) =>
      map2((|>), a, b, json);

    let required = (name, decode) =>
      pipe(decodeField(name, decode));

    let optional = (name, decode) =>
      pipe(decodeOptionalField(name, decode));

    let fallback = (name, decode, fallback) =>
      pipe(decodeFieldWithFallback(name, decode, fallback));

    let hardcoded = v =>
      pipe(succeed(v));

    /**
     * `run` takes a decoder and some json, and it passes that json to the
     * decoder. The result is that your decoder is run with the provided json
     *
     * The actual signature looks something like this:
     * (json => t), json => t
     *
     * ...which, turns out, is `identity`.
     *
     */
    let run = BsAbstract.Functions.id;
  }
};
