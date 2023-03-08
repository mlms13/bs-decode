module NonEmptyList: Decode_NonEmptyList.Nel;
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

let map2:
  (
    ('a, 'b) => 'c,
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t
  ) =>
  result('c, ParseError.failure);

let map3:
  (
    ('a, 'b, 'c) => 'd,
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t
  ) =>
  result('d, ParseError.failure);

let map4:
  (
    ('a, 'b, 'c, 'd) => 'e,
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t => result('d, ParseError.failure),
    Js.Json.t
  ) =>
  result('e, ParseError.failure);

let map5:
  (
    ('a, 'b, 'c, 'd, 'e) => 'f,
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t => result('d, ParseError.failure),
    Js.Json.t => result('e, ParseError.failure),
    Js.Json.t
  ) =>
  result('f, ParseError.failure);

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
let boolean: Js.Json.t => result(bool, ParseError.failure);
let string: Js.Json.t => result(string, ParseError.failure);
let floatFromNumber: Js.Json.t => result(float, ParseError.failure);
let intFromNumber: Js.Json.t => result(int, ParseError.failure);
let date: Js.Json.t => result(Js.Date.t, ParseError.failure);
let variantFromJson:
  (
    Js.Json.t => result('a, ParseError.failure),
    'a => option('b),
    Js.Json.t
  ) =>
  result('b, ParseError.failure);
let variantFromString:
  (string => option('a), Js.Json.t) => result('a, ParseError.failure);
let variantFromInt:
  (int => option('a), Js.Json.t) => result('a, ParseError.failure);

let optional:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(option('a), ParseError.failure);

let array:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(array('a), ParseError.failure);

let list:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(list('a), ParseError.failure);

let tuple:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b), ParseError.failure);

let tuple2:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b), ParseError.failure);

let tuple3:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c), ParseError.failure);

let tuple4:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t => result('d, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd), ParseError.failure);

let tuple5:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t => result('d, ParseError.failure),
    Js.Json.t => result('e, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd, 'e), ParseError.failure);

let tupleAtLeast2:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b), ParseError.failure);

let tupleAtLeast3:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c), ParseError.failure);

let tupleAtLeast4:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t => result('d, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd), ParseError.failure);

let tupleAtLeast5:
  (
    Js.Json.t => result('a, ParseError.failure),
    Js.Json.t => result('b, ParseError.failure),
    Js.Json.t => result('c, ParseError.failure),
    Js.Json.t => result('d, ParseError.failure),
    Js.Json.t => result('e, ParseError.failure),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd, 'e), ParseError.failure);

let tupleFromFields:
  (
    (string, Js.Json.t => result('a, ParseError.failure)),
    (string, Js.Json.t => result('b, ParseError.failure)),
    Js.Json.t
  ) =>
  result(('a, 'b), ParseError.failure);

let dict:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(Js.Dict.t('a), ParseError.failure);

let stringMap:
  (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
  result(Belt.Map.String.t('a), ParseError.failure);

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

[@deprecated "Will be removed in favor up the upcoming addition of letops"]
module Pipeline: {
  let succeed: ('a, Js.Json.t) => result('a, ParseError.failure);

  let pipe:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);

  let field:
    (
      string,
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);

  let at:
    (
      list(string),
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);

  let optionalField:
    (
      string,
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result(option('a) => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);

  let fallbackField:
    (
      string,
      Js.Json.t => result('a, ParseError.failure),
      'a,
      Js.Json.t => result('a => 'b, ParseError.failure),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);

  let fallback:
    (Js.Json.t => result('a, ParseError.failure), 'a, Js.Json.t) =>
    result('a, ParseError.failure);

  let hardcoded:
    ('a, Js.Json.t => result('a => 'c, ParseError.failure), Js.Json.t) =>
    result('c, ParseError.failure);

  let run:
    (Js.Json.t, Js.Json.t => result('a, ParseError.failure)) =>
    result('a, ParseError.failure);

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

  let map2:
    (
      ('a, 'b) => 'c,
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t
    ) =>
    result('c, ParseError.failure);

  let map3:
    (
      ('a, 'b, 'c) => 'd,
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t
    ) =>
    result('d, ParseError.failure);

  let map4:
    (
      ('a, 'b, 'c, 'd) => 'e,
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t => result('d, ParseError.failure),
      Js.Json.t
    ) =>
    result('e, ParseError.failure);

  let map5:
    (
      ('a, 'b, 'c, 'd, 'e) => 'f,
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t => result('d, ParseError.failure),
      Js.Json.t => result('e, ParseError.failure),
      Js.Json.t
    ) =>
    result('f, ParseError.failure);

  let pure: ('a, Js.Json.t) => result('a, ParseError.failure);

  let flatMap:
    (
      ('a, Js.Json.t) => result('b, ParseError.failure),
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);

  let boolean: Js.Json.t => result(bool, ParseError.failure);
  let string: Js.Json.t => result(Js.String.t, ParseError.failure);
  let floatFromNumber: Js.Json.t => result(float, ParseError.failure);
  let intFromNumber: Js.Json.t => result(int, ParseError.failure);
  let date: Js.Json.t => result(Js.Date.t, ParseError.failure);
  let variantFromJson:
    (
      Js.Json.t => result('a, ParseError.failure),
      'a => option('b),
      Js.Json.t
    ) =>
    result('b, ParseError.failure);
  let variantFromString:
    (string => option('a), Js.Json.t) => result('a, ParseError.failure);
  let variantFromInt:
    (int => option('a), Js.Json.t) => result('a, ParseError.failure);

  let optional:
    (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
    result(option('a), ParseError.failure);

  let array:
    (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
    result(array('a), ParseError.failure);

  let list:
    (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
    result(list('a), ParseError.failure);

  let tuple:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b), ParseError.failure);

  let tuple2:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b), ParseError.failure);

  let tuple3:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c), ParseError.failure);

  let tuple4:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t => result('d, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd), ParseError.failure);

  let tuple5:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t => result('d, ParseError.failure),
      Js.Json.t => result('e, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd, 'e), ParseError.failure);

  let tupleAtLeast2:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b), ParseError.failure);

  let tupleAtLeast3:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c), ParseError.failure);

  let tupleAtLeast4:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t => result('d, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd), ParseError.failure);

  let tupleAtLeast5:
    (
      Js.Json.t => result('a, ParseError.failure),
      Js.Json.t => result('b, ParseError.failure),
      Js.Json.t => result('c, ParseError.failure),
      Js.Json.t => result('d, ParseError.failure),
      Js.Json.t => result('e, ParseError.failure),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd, 'e), ParseError.failure);

  let tupleFromFields:
    (
      (string, Js.Json.t => result('a, ParseError.failure)),
      (string, Js.Json.t => result('b, ParseError.failure)),
      Js.Json.t
    ) =>
    result(('a, 'b), ParseError.failure);

  let dict:
    (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
    result(Js.Dict.t('a), ParseError.failure);

  let stringMap:
    (Js.Json.t => result('a, ParseError.failure), Js.Json.t) =>
    result(Belt.Map.String.t('a), ParseError.failure);

  let oneOf:
    (
      Js.Json.t => result('a, ParseError.failure),
      list(Js.Json.t => result('a, ParseError.failure)),
      Js.Json.t
    ) =>
    result('a, ParseError.failure);
};
