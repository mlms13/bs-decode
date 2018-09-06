type decoder('a) = Decoder(Js.Json.t => Belt.Result.t('a, DecodeError.t));
let make = v => Decoder(v);
let succeed = v => Decoder(_ => Belt.Result.Ok(v));

let map2 = (fn, Decoder(decodeA), Decoder(decodeB)) =>
  Decoder(json => ResultDecodeError.map2(fn, decodeA(json), decodeB(json)));

let field = (name, Decoder(decoder)) =>
  Decoder(Decode.decodeField(name, decoder));

let required = (name, a, fn) =>
  map2((|>), field(name, a), fn);

let run = (Decoder(decoder), json) =>
  decoder(json);
