module type TransformError = {
  type t('a);
  let transform: DecodeFailure.t => t('a);
  let mapErr: ((DecodeFailure.t => DecodeFailure.t), t('a)) => t('a);
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

  open DecodeFailure;

  let ok = v => M.pure(v);
  let err = x => T.transform(x);
  let map2 = (f, a, b) => f <$> a <*> b;
  let mapErr = T.mapErr;
  let fromOpt = (fn, default, opt) =>
    BsAbstract.Option.maybe(~f=fn, ~default, opt);

  let decodeVal = (decode, failure, json) =>
    decode(json) |> fromOpt(ok, err(Val(failure, json)));

  let decodeString =
    decodeVal(Js.Json.decodeString, ExpectedString);

  let decodeFloat = json =>
    decodeVal(Js.Json.decodeNumber, ExpectedNumber, json);

  let decodeInt = json => {
    let isInt = v => mod_float(v, floor(v)) == 0.;
    decodeFloat(json) |. M.flat_map(v =>
      if (isInt(v)) ok(int_of_float(v))
      else err(Val(ExpectedInt, json))
    );
  };

  let decodeOpt = (decode, json) =>
    switch (Js.Json.decodeNull(json)) {
    | Some(_) => ok(None)
    | None => decode(json) |> M.map(v => Some(v))
    };

  let decodeArray = (decode, json) => {
    let decodeEach = arr => BsAbstract.Array.Foldable.fold_left(((pos, acc), curr) => {
      let decoded = decode(curr) |> mapErr(v => Arr(NonEmptyList.pure((pos, v))));
      let result = map2((arr, v) => Array.append(arr, [|v|]), acc, decoded);
      (pos + 1, result);
    }, (0, ok([||])), arr) |> snd;

    decodeVal(Js.Json.decodeArray, ExpectedArray, json)
      |. M.flat_map(decodeEach);
  };

  let decodeList = (decode, json) =>
    decodeArray(decode, json) |> M.map(Array.to_list);

  let rec decodeAt = (fields, decode, json) => switch fields {
  | [] => decode(json)
  | [x, ...xs] =>
    decodeVal(Js.Json.decodeObject, ExpectedObject, json)
      |> M.map(Js.Dict.get(_, x))
      |. M.flat_map(fromOpt(ok, err(objPure(x, MissingField))))
      |. M.flat_map(v => decodeAt(xs, decode, v) |> mapErr(err => objPure(x, InvalidField(err))));
  };

  let decodeField = (name, decode, json) =>
    decodeAt([name], decode, json);

  let decodeFieldWithFallback = (name, decode, fallback, json) =>
    decodeField(name, decode, json) <|> M.pure(fallback);

  let decodeOptionalField = (name, decode, json) =>
    decodeVal(Js.Json.decodeObject, ExpectedObject, json)
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
};
