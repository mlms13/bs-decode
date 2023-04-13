module ParseError = Decode_ParseError;

let map:
  ('a => 'b, Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result('b, ParseError.failure);

let apply:
  (
    Js.Json.t => result('a => 'b, ParseError.failure),
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t
  ) =>
  result('b, ParseError.failure);

let pair:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b), ParseError.failure);

let (let+):
  (Js.Json.t => result('a, ParseError.failure), 'a => 'b, Js.Json.t) =>
  result('b, ParseError.failure);

let (and+):
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b), ParseError.failure);

let pure: ('a, Js.Json.t) => result('a, ParseError.failure);

let flatMap:
  (
    ('a, Js.Json.t) => result('b, ParseError.failure),
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t
  ) =>
  result('b, ParseError.failure);

let alt:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t
  ) =>
  result('a, ParseError.failure);

let okJson: Js.Json.t => result(Js.Json.t, ParseError.failure);
let null: Js.Json.t => result(unit, ParseError.failure);
let boolean: Js.Json.t => result(bool, ParseError.failure);
let string: Js.Json.t => result(string, ParseError.failure);
let floatFromNumber: Js.Json.t => result(float, ParseError.failure);
let intFromNumber: Js.Json.t => result(int, ParseError.failure);
let date: Js.Json.t => result(Js.Date.t, ParseError.failure);
let literal:
  (
    ('a, 'a) => bool,
    Js.Json.t => result('a, ParseError.failure),
    'a,
    Js.Json.t
  ) =>
  result('a, ParseError.failure);
let literalString: (string, Js.Json.t) => result(string, ParseError.failure);
let literalInt: (int, Js.Json.t) => result(int, ParseError.failure);
let literalFloat: (float, Js.Json.t) => result(float, ParseError.failure);
let literalBool: (bool, Js.Json.t) => result(bool, ParseError.failure);
let literalTrue: Js.Json.t => result(bool, ParseError.failure);
let literalFalse: Js.Json.t => result(bool, ParseError.failure);

let union:
  (
    ('a, Js.Json.t) => result('a, ParseError.failure),
    ('a, 'b),
    list(('a, 'b)),
    Js.Json.t
  ) =>
  result('b, ParseError.failure);

let stringUnion:
  ((string, 'a), list((string, 'a)), Js.Json.t) =>
  result('a, ParseError.failure);

let intUnion:
  ((int, 'a), list((int, 'a)), Js.Json.t) => result('a, ParseError.failure);

let optional:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(option('a), ParseError.failure);

let array:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(array('a), ParseError.failure);

let arrayJson: Js.Json.t => result(array(Js.Json.t), ParseError.failure);

let list:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(list('a), ParseError.failure);

let listJson: Js.Json.t => result(list(Js.Json.t), ParseError.failure);

let arrayAt:
  (int, Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result('a, ParseError.failure);

let dict:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(Js.Dict.t('a), ParseError.failure);

let dictJson: Js.Json.t => result(Js.Dict.t(Js.Json.t), ParseError.failure);

let at:
  (list(string), Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result('a, ParseError.failure);

let field:
  (string, Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result('a, ParseError.failure);

let optionalField:
  (string, Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(option('a), ParseError.failure);

let fallback:
  (Js.Json.t => result('a, ParseError.failure), 'a, Js.Json.t) =>
  result('a, ParseError.failure);

let oneOf:
  (
    Js.Json.t => result('a, ParseError.failure),
    list(Js.Json.t => result('a, ParseError.failure)),
    Js.Json.t
  ) =>
  result('a, ParseError.failure);

let hush:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) => option('a);
