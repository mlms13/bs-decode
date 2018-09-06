/* Result helpers */
let ((<$>), (<*>)) = ResultDecodeError.((<$>), (<*>));

/**
 * `succeed` returns a `json => Result` decode function that ignores the `json`
 * argument and always returns `Ok`
 */
let succeed = v => BsAbstract.Function.const(Belt.Result.Ok(v));

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

let required = (name, a) =>
  pipe(Decode.decodeField(name, a));

let run = BsAbstract.Functions.id;
