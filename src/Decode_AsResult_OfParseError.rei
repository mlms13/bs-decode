module NonEmptyList: Decode_NonEmptyList.Nel;
module ParseError = Decode_ParseError;

let map:
  ('a => 'b, Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t('b, ParseError.failure);

let apply:
  (
    Js.Json.t => Belt.Result.t('a => 'b, ParseError.failure),
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('b, ParseError.failure);

let map2:
  (
    ('a, 'b) => 'c,
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('c, ParseError.failure);

let map3:
  (
    ('a, 'b, 'c) => 'd,
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('d, ParseError.failure);

let map4:
  (
    ('a, 'b, 'c, 'd) => 'e,
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t => Belt.Result.t('d, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('e, ParseError.failure);

let map5:
  (
    ('a, 'b, 'c, 'd, 'e) => 'f,
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t => Belt.Result.t('d, ParseError.failure),
    Js.Json.t => Belt.Result.t('e, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('f, ParseError.failure);

let pure: ('a, Js.Json.t) => Belt.Result.t('a, ParseError.failure);

let flatMap:
  (
    ('a, Js.Json.t) => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('b, ParseError.failure);

let alt:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('a, ParseError.failure);

let boolean: Js.Json.t => Belt.Result.t(bool, ParseError.failure);
let string: Js.Json.t => Belt.Result.t(string, ParseError.failure);
let floatFromNumber: Js.Json.t => Belt.Result.t(float, ParseError.failure);
let intFromNumber: Js.Json.t => Belt.Result.t(int, ParseError.failure);
let date: Js.Json.t => Belt.Result.t(Js.Date.t, ParseError.failure);
let variantFromJson:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    'a => option('b),
    Js.Json.t
  ) =>
  Belt.Result.t('b, ParseError.failure);
let variantFromString:
  (string => option('a), Js.Json.t) => Belt.Result.t('a, ParseError.failure);
let variantFromInt:
  (int => option('a), Js.Json.t) => Belt.Result.t('a, ParseError.failure);

let optional:
  (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t(option('a), ParseError.failure);

let array:
  (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t(array('a), ParseError.failure);

let list:
  (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t(list('a), ParseError.failure);

let tuple:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), ParseError.failure);

let tuple2:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), ParseError.failure);

let tuple3:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b, 'c), ParseError.failure);

let tuple4:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t => Belt.Result.t('d, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b, 'c, 'd), ParseError.failure);

let tuple5:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t => Belt.Result.t('d, ParseError.failure),
    Js.Json.t => Belt.Result.t('e, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b, 'c, 'd, 'e), ParseError.failure);

let tupleAtLeast2:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), ParseError.failure);

let tupleAtLeast3:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b, 'c), ParseError.failure);

let tupleAtLeast4:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t => Belt.Result.t('d, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b, 'c, 'd), ParseError.failure);

let tupleAtLeast5:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t => Belt.Result.t('b, ParseError.failure),
    Js.Json.t => Belt.Result.t('c, ParseError.failure),
    Js.Json.t => Belt.Result.t('d, ParseError.failure),
    Js.Json.t => Belt.Result.t('e, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b, 'c, 'd, 'e), ParseError.failure);

let tupleFromFields:
  (
    (string, Js.Json.t => Belt.Result.t('a, ParseError.failure)),
    (string, Js.Json.t => Belt.Result.t('b, ParseError.failure)),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), ParseError.failure);

let dict:
  (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t(Js.Dict.t('a), ParseError.failure);

let stringMap:
  (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t(Belt.Map.String.t('a), ParseError.failure);


let at:
  (
    list(string),
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('a, ParseError.failure);

let field:
  (string, Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t('a, ParseError.failure);

let optionalField:
  (string, Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
  Belt.Result.t(option('a), ParseError.failure);

let fallback:
  (Js.Json.t => Belt.Result.t('a, ParseError.failure), 'a, Js.Json.t) =>
  Belt.Result.t('a, ParseError.failure);

let oneOf:
  (
    Js.Json.t => Belt.Result.t('a, ParseError.failure),
    list(Js.Json.t => Belt.Result.t('a, ParseError.failure)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, ParseError.failure);

module Pipeline: {
  let succeed: ('a, Js.Json.t) => Belt.Result.t('a, ParseError.failure);

  let field:
    (
      string,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let at:
    (
      list(string),
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let optionalField:
    (
      string,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t(option('a) => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let fallbackField:
    (
      string,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      'a,
      Js.Json.t => Belt.Result.t('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let fallback:
    (Js.Json.t => Belt.Result.t('a, ParseError.failure), 'a, Js.Json.t) =>
    Belt.Result.t('a, ParseError.failure);

  let hardcoded:
    (
      'a,
      Js.Json.t => Belt.Result.t('a => 'c, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('c, ParseError.failure);

  let run:
    (Js.Json.t, Js.Json.t => Belt.Result.t('a, ParseError.failure)) =>
    Belt.Result.t('a, ParseError.failure);

  let map:
    (
      'a => 'b,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let apply:
    (
      Js.Json.t => Belt.Result.t('a => 'b, ParseError.failure),
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let map2:
    (
      ('a, 'b) => 'c,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t('b, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('c, ParseError.failure);

  let map3:
    (
      ('a, 'b, 'c) => 'd,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t('b, ParseError.failure),
      Js.Json.t => Belt.Result.t('c, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('d, ParseError.failure);

  let map4:
    (
      ('a, 'b, 'c, 'd) => 'e,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t('b, ParseError.failure),
      Js.Json.t => Belt.Result.t('c, ParseError.failure),
      Js.Json.t => Belt.Result.t('d, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('e, ParseError.failure);

  let map5:
    (
      ('a, 'b, 'c, 'd, 'e) => 'f,
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t => Belt.Result.t('b, ParseError.failure),
      Js.Json.t => Belt.Result.t('c, ParseError.failure),
      Js.Json.t => Belt.Result.t('d, ParseError.failure),
      Js.Json.t => Belt.Result.t('e, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('f, ParseError.failure);

  let pure: ('a, Js.Json.t) => Belt.Result.t('a, ParseError.failure);

  let flatMap:
    (
      ('a, Js.Json.t) => Belt.Result.t('b, ParseError.failure),
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);

  let boolean: Js.Json.t => Belt.Result.t(bool, ParseError.failure);
  let string: Js.Json.t => Belt.Result.t(Js.String.t, ParseError.failure);
  let floatFromNumber: Js.Json.t => Belt.Result.t(float, ParseError.failure);
  let intFromNumber: Js.Json.t => Belt.Result.t(int, ParseError.failure);
  let date: Js.Json.t => Belt.Result.t(Js.Date.t, ParseError.failure);
  let variantFromJson:
    (
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      'a => option('b),
      Js.Json.t
    ) =>
    Belt.Result.t('b, ParseError.failure);
  let variantFromString:
    (string => option('a), Js.Json.t) =>
    Belt.Result.t('a, ParseError.failure);
  let variantFromInt:
    (int => option('a), Js.Json.t) => Belt.Result.t('a, ParseError.failure);

  let optional:
    (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
    Belt.Result.t(option('a), ParseError.failure);

  let array:
    (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
    Belt.Result.t(array('a), ParseError.failure);

  let list:
    (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
    Belt.Result.t(list('a), ParseError.failure);

  let tupleFromFields:
    (
      (string, Js.Json.t => Belt.Result.t('a, ParseError.failure)),
      (string, Js.Json.t => Belt.Result.t('b, ParseError.failure)),
      Js.Json.t
    ) =>
    Belt.Result.t(('a, 'b), ParseError.failure);

  let dict:
    (Js.Json.t => Belt.Result.t('a, ParseError.failure), Js.Json.t) =>
    Belt.Result.t(Js.Dict.t('a), ParseError.failure);

  let oneOf:
    (
      Js.Json.t => Belt.Result.t('a, ParseError.failure),
      list(Js.Json.t => Belt.Result.t('a, ParseError.failure)),
      Js.Json.t
    ) =>
    Belt.Result.t('a, ParseError.failure);
};
