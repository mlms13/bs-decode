open Relude.Globals;
open BsBastet.Interface;

module ParseError = Decode_ParseError;

[@ocaml.warning "-3"]
module Make =
       (T: ParseError.TransformError, M: MONAD with type t('a) = T.t('a)) => {
  type t('a) = Js.Json.t => M.t('a);

  let map = (f, decode) => decode >> M.map(f);
  let apply = (f, decode, json) => M.apply(f(json), decode(json));
  let pure = (v, _) => M.pure(v);
  let flat_map = (decode, f, json) => M.flat_map(decode(json), f(_, json));
  let alt = (a, b, json) => T.lazyAlt(a(json), () => b(json));

  let pair = (da, db) => da |> map((a, b) => (a, b)) |> apply(_, db);

  let (let+) = (a, f) => map(f, a);
  let (and+) = pair;
  let ( let* ) = flat_map;

  include Relude.Extensions.Apply.ApplyExtensions({
    type nonrec t('a) = t('a);
    let map = map;
    let apply = apply;
  });

  let flatMap = (f, decode) => flat_map(decode, f);

  let oneOf = (decode, rest) => List.foldLeft(alt, decode, rest);

  let value = (decode, failure, json) =>
    decode(json) |> Option.foldLazy(() => T.valErr(failure, json), M.pure);

  let okJson = M.pure;

  let null = value(Js.Json.decodeNull, `ExpectedNull) |> map(_ => ());

  let boolean = value(Js.Json.decodeBoolean, `ExpectedBoolean);

  let string = value(Js.Json.decodeString, `ExpectedString);

  let floatFromNumber = value(Js.Json.decodeNumber, `ExpectedNumber);

  let intFromNumber = {
    let isInt = num => float(int_of_float(num)) == num;
    flatMap(
      v => isInt(v) ? pure(int_of_float(v)) : T.valErr(`ExpectedInt),
      floatFromNumber,
    );
  };

  let date = {
    let fromFloat = map(Js.Date.fromFloat, floatFromNumber);
    let fromString = map(Js.Date.fromString, string);
    let isValid = date =>
      date
      |> Js.Date.toJSONUnsafe
      |> Js.Nullable.return
      |> Js.Nullable.isNullable
        ? T.valErr(`ExpectedValidDate) : pure(date);

    alt(fromFloat, fromString) |> flatMap(isValid);
  };

  let literal = (eq, decode, a) =>
    decode
    |> flatMap(v => eq(v, a) ? pure(a) : T.valErr(`ExpectedValidOption));

  let literalString = literal((==), string);

  let literalInt = literal((==), intFromNumber);

  let literalFloat = literal((==), floatFromNumber);

  let literalBool = literal((==), boolean);

  let literalTrue = literalBool(true);

  let literalFalse = literalBool(false);

  let union = (decode, first, rest) => {
    let mkDecode = ((k, v)) => decode(k) |> map(_ => v);
    oneOf(mkDecode(first), rest |> List.map(mkDecode));
  };

  let stringUnion = first => union(literalString, first);

  let intUnion = first => union(literalInt, first);

  let optional = (decode, json) =>
    switch (Js.Json.decodeNull(json)) {
    | Some(_) => pure(None, json)
    | None => map(v => Some(v), decode, json)
    };

  let array = decode => {
    let map2 = (f, a, b) => M.map(f, a) |> M.apply(_, b);
    let decodeEach = (arr, _json) =>
      Array.foldLeft(
        ((pos, acc), curr) => {
          let decoded = T.arrErr(pos, decode(curr));
          let result = map2(flip(Array.append), acc, decoded);
          (pos + 1, result);
        },
        (0, M.pure([||])),
        arr,
      )
      |> snd;

    value(Js.Json.decodeArray, `ExpectedArray) |> flatMap(decodeEach);
  };

  let arrayJson = array(okJson);

  let list = decode => array(decode) |> map(Array.toList);

  let listJson = list(okJson);

  let arrayAt = (position, decode) =>
    array(okJson)
    |> flatMap((arr, json) =>
         arr
         |> Array.at(position)
         |> Option.foldLazy(
              () => T.valErr(`ExpectedTuple(position + 1), json),
              decode,
            )
       );

  let tupleN = (size, extract, decode) =>
    array(M.pure)
    |> map(extract)
    |> flatMap(Option.fold(T.valErr(`ExpectedTuple(size)), decode));

  let tuple = (da, db) =>
    tupleN(2, Tuple.fromArray, ((a, b)) =>
      map2(Tuple.make, _ => T.arrErr(0, da(a)), _ => T.arrErr(1, db(b)))
    );

  let tuple2 = tuple;

  let tuple3 = (da, db, dc) =>
    tupleN(3, Tuple.fromArray3, ((a, b, c)) =>
      map3(Tuple.make3, _ => da(a), _ => db(b), _ => dc(c))
    );

  let tuple4 = (da, db, dc, dd) =>
    tupleN(4, Tuple.fromArray4, ((a, b, c, d)) =>
      map4(Tuple.make4, _ => da(a), _ => db(b), _ => dc(c), _ => dd(d))
    );

  let tuple5 = (da, db, dc, dd, de) =>
    tupleN(5, Tuple.fromArray5, ((a, b, c, d, e)) =>
      map5(
        Tuple.make5,
        _ => da(a),
        _ => db(b),
        _ => dc(c),
        _ => dd(d),
        _ => de(e),
      )
    );

  let tupleAtLeast2 = (da, db) =>
    tupleN(2, Tuple.fromArrayAtLeast2, ((a, b)) =>
      map2(Tuple.make2, _ => da(a), _ => db(b))
    );

  let tupleAtLeast3 = (da, db, dc) =>
    tupleN(3, Tuple.fromArrayAtLeast3, ((a, b, c)) =>
      map3(Tuple.make3, _ => da(a), _ => db(b), _ => dc(c))
    );

  let tupleAtLeast4 = (da, db, dc, dd) =>
    tupleN(4, Tuple.fromArrayAtLeast4, ((a, b, c, d)) =>
      map4(Tuple.make4, _ => da(a), _ => db(b), _ => dc(c), _ => dd(d))
    );

  let tupleAtLeast5 = (da, db, dc, dd, de) =>
    tupleN(5, Tuple.fromArrayAtLeast5, ((a, b, c, d, e)) =>
      map5(
        Tuple.make5,
        _ => da(a),
        _ => db(b),
        _ => dc(c),
        _ => dd(d),
        _ => de(e),
      )
    );

  let dict = decode => {
    let rec decodeEntries =
      fun
      | [] => pure([])
      | [(key, value), ...xs] =>
        map2(
          (decodedValue, rest) => [(key, decodedValue), ...rest],
          _ => T.objErr(key, decode(value)),
          decodeEntries(xs),
        );

    value(Js.Json.decodeObject, `ExpectedObject)
    |> map(Js.Dict.entries >> Array.toList)
    |> flatMap(decodeEntries)
    |> map(Js.Dict.fromList);
  };

  let dictJson = dict(okJson);

  let stringMap = decode =>
    dict(decode) |> map(Js.Dict.entries) |> map(Belt.Map.String.fromArray);

  let rec at = (fields, decode) =>
    switch (fields) {
    | [] => decode
    | [x, ...xs] =>
      value(Js.Json.decodeObject, `ExpectedObject)
      |> map(Js.Dict.get(_, x))
      |> flatMap(Option.fold(_ => T.missingFieldErr(x), pure))
      |> flatMap(at(xs, decode) >> T.objErr(x) >> const)
    };

  let field = (name, decode) => at([name], decode);

  let optionalField = (name, decode) =>
    value(Js.Json.decodeObject, `ExpectedObject)
    |> map(Js.Dict.get(_, name))
    |> flatMap(
         fun
         | None => pure(None)
         | Some(v) => (_ => optional(decode, v) |> T.objErr(name)),
       );

  let fallback = (decode, recovery) => alt(decode, pure(recovery));

  let tupleFromFields = ((fieldA, decodeA), (fieldB, decodeB)) =>
    map2(Tuple.make, field(fieldA, decodeA), field(fieldB, decodeB));

  let hush = (decode, json) => decode(json) |> Result.toOption;
};
