type failure = [
  | `ExpectedBoolean
  | `ExpectedString
  | `ExpectedNumber
  | `ExpectedInt
  | `ExpectedArray
  | `ExpectedObject
];

let failureToString = (v, json) =>
  (
    switch (v) {
    | `ExpectedBoolean => "Expected boolean"
    | `ExpectedString => "Expected string"
    | `ExpectedNumber => "Expected number"
    | `ExpectedInt => "Expected int"
    | `ExpectedArray => "Expected array"
    | `ExpectedObject => "Expected object"
    }
  )
  ++ " but found "
  ++ Js.Json.stringify(json);

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
  module InfixMonad = BsAbstract.Infix.Monad(M);
  module InfixAlt = BsAbstract.Infix.Alt(Alt);

  let ((<$>), (<*>), (>>=)) = InfixMonad.((<$>), (<*>), (>>=));
  let ((<|>), (<#>)) = InfixAlt.((<|>), (<#>));
  let (>.) = BsAbstract.Function.Infix.(>.);

  let ok = v => M.pure(v);
  let map2 = (f, a, b) => f <$> a <*> b;
  let fromOpt = (fn, default, opt) =>
    BsAbstract.Option.maybe(~f=fn, ~default, opt);

  let value = (decode, failure, json) =>
    decode(json) |> fromOpt(ok, T.valErr(failure, json));

  let boolean = value(Js.Json.decodeBoolean, `ExpectedBoolean);

  let string = value(Js.Json.decodeString, `ExpectedString);

  let float = json => value(Js.Json.decodeNumber, `ExpectedNumber, json);

  let int = json => {
    let isInt = v => v == 0. || mod_float(v, floor(v)) == 0.;
    float(json)
    ->(
        M.flat_map(v =>
          if (isInt(v)) {
            ok(int_of_float(v));
          } else {
            T.valErr(`ExpectedInt, json);
          }
        )
      );
  };

  let optional = (decode, json) =>
    switch (Js.Json.decodeNull(json)) {
    | Some(_) => ok(None)
    | None => decode(json) |> M.map(v => Some(v))
    };

  let array = (decode, json) => {
    let decodeEach = arr =>
      BsAbstract.Array.Foldable.fold_left(
        ((pos, acc), curr) => {
          let decoded = T.arrErr(pos, decode(curr));
          let result =
            map2((arr, v) => Array.append(arr, [|v|]), acc, decoded);
          (pos + 1, result);
        },
        (0, ok([||])),
        arr,
      )
      |> snd;

    value(Js.Json.decodeArray, `ExpectedArray, json)
    ->(M.flat_map(decodeEach));
  };

  let list = (decode, json) => array(decode, json) |> M.map(Array.to_list);

  let dict = (decode, json) => {
    let rec decodeEntries = entries =>
      switch (entries) {
      | [] => []->ok
      | [(key, value), ...xs] =>
        map2(
          (decodedValue, rest) => [(key, decodedValue), ...rest],
          T.objErr(key, value->decode),
          xs->decodeEntries,
        )
      };

    value(Js.Json.decodeObject, `ExpectedObject, json)
    <#> Js.Dict.entries
    <#> Belt.List.fromArray
    >>= decodeEntries
    <#> Js.Dict.fromList;
  };

  let rec at = (fields, decode, json) =>
    switch (fields) {
    | [] => decode(json)
    | [x, ...xs] =>
      (
        value(Js.Json.decodeObject, `ExpectedObject, json)
        |> M.map(Js.Dict.get(_, x))
      )
      ->(M.flat_map(fromOpt(ok, T.missingFieldErr(x))))
      ->(M.flat_map(v => T.objErr(x, at(xs, decode, v))))
    };

  let field = (name, decode, json) => at([name], decode, json);

  let optionalField = (name, decode, json) =>
    (
      value(Js.Json.decodeObject, `ExpectedObject, json)
      |> M.map(Js.Dict.get(_, name))
    )
    ->(
        M.flat_map(opt =>
          switch (opt) {
          | None => ok(None)
          | Some(v) => optional(decode, v)
          }
        )
      );

  let fallback = (name, decode, alt, json) =>
    field(name, decode, json) <|> M.pure(alt);

  let tuple = ((fieldA, decodeA), (fieldB, decodeB), json) =>
    map2(
      (a, b) => (a, b),
      field(fieldA, decodeA, json),
      field(fieldB, decodeB, json),
    );

  let rec oneOf = (decoders, json) =>
    switch (decoders) {
    | NonEmptyList.NonEmpty(decode, []) => decode(json)
    | NonEmptyList.NonEmpty(decode, [y, ...ys]) =>
      T.lazyAlt(decode(json), () => oneOf(NonEmptyList.make(y, ys), json))
    };

  module Pipeline = {
    /**
     * `succeed` returns a `json => Result` decode function that ignores the `json`
     * argument and always returns `Ok`
     */
    let succeed = (v, _) => ok(v);

    let map2 = (f, a, b, json) => f <$> a(json) <*> b(json);

    let map3 = (f, a, b, c, json) => f <$> a(json) <*> b(json) <*> c(json);

    let map4 = (f, a, b, c, d, json) =>
      f <$> a(json) <*> b(json) <*> c(json) <*> d(json);

    let map5 = (f, a, b, c, d, e, json) =>
      f <$> a(json) <*> b(json) <*> c(json) <*> d(json) <*> e(json);

    let pipe = (a, b, json) => map2((|>), a, b, json);

    let field = (name, decode) => pipe(field(name, decode));

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
