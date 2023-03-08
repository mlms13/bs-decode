module NonEmptyList: Decode_NonEmptyList.Nel;

let map:
  ('a => 'b, Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result('b, NonEmptyList.t(string));

let apply:
  (
    Js.Json.t => result('a => 'b, NonEmptyList.t(string)),
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('b, NonEmptyList.t(string));

let map2:
  (
    ('a, 'b) => 'c,
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('c, NonEmptyList.t(string));

let map3:
  (
    ('a, 'b, 'c) => 'd,
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('d, NonEmptyList.t(string));

let map4:
  (
    ('a, 'b, 'c, 'd) => 'e,
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t => result('d, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('e, NonEmptyList.t(string));

let map5:
  (
    ('a, 'b, 'c, 'd, 'e) => 'f,
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t => result('d, NonEmptyList.t(string)),
    Js.Json.t => result('e, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('f, NonEmptyList.t(string));

let pure: ('a, Js.Json.t) => result('a, NonEmptyList.t(string));

let flatMap:
  (
    ('a, Js.Json.t) => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('b, NonEmptyList.t(string));

let alt:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('a, NonEmptyList.t(string));

let okJson: Js.Json.t => result(Js.Json.t, NonEmptyList.t(string));
let boolean: Js.Json.t => result(bool, NonEmptyList.t(string));
let string: Js.Json.t => result(string, NonEmptyList.t(string));
let floatFromNumber: Js.Json.t => result(float, NonEmptyList.t(string));
let intFromNumber: Js.Json.t => result(int, NonEmptyList.t(string));
let date: Js.Json.t => result(Js.Date.t, NonEmptyList.t(string));
let variantFromJson:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    'a => option('b),
    Js.Json.t
  ) =>
  result('b, NonEmptyList.t(string));
let variantFromString:
  (string => option('a), Js.Json.t) => result('a, NonEmptyList.t(string));
let variantFromInt:
  (int => option('a), Js.Json.t) => result('a, NonEmptyList.t(string));

let optional:
  (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result(option('a), NonEmptyList.t(string));

let array:
  (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result(array('a), NonEmptyList.t(string));

let list:
  (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result(list('a), NonEmptyList.t(string));

let tuple:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b), NonEmptyList.t(string));

let tuple2:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b), NonEmptyList.t(string));

let tuple3:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c), NonEmptyList.t(string));

let tuple4:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t => result('d, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd), NonEmptyList.t(string));

let tuple5:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t => result('d, NonEmptyList.t(string)),
    Js.Json.t => result('e, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd, 'e), NonEmptyList.t(string));

let tupleAtLeast2:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b), NonEmptyList.t(string));

let tupleAtLeast3:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c), NonEmptyList.t(string));

let tupleAtLeast4:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t => result('d, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd), NonEmptyList.t(string));

let tupleAtLeast5:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t => result('b, NonEmptyList.t(string)),
    Js.Json.t => result('c, NonEmptyList.t(string)),
    Js.Json.t => result('d, NonEmptyList.t(string)),
    Js.Json.t => result('e, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result(('a, 'b, 'c, 'd, 'e), NonEmptyList.t(string));

let tupleFromFields:
  (
    (string, Js.Json.t => result('a, NonEmptyList.t(string))),
    (string, Js.Json.t => result('b, NonEmptyList.t(string))),
    Js.Json.t
  ) =>
  result(('a, 'b), NonEmptyList.t(string));

let dict:
  (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result(Js.Dict.t('a), NonEmptyList.t(string));

let stringMap:
  (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result(Belt.Map.String.t('a), NonEmptyList.t(string));

let at:
  (
    list(string),
    Js.Json.t => result('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  result('a, NonEmptyList.t(string));

let field:
  (string, Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result('a, NonEmptyList.t(string));

let optionalField:
  (string, Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
  result(option('a), NonEmptyList.t(string));

let fallback:
  (Js.Json.t => result('a, NonEmptyList.t(string)), 'a, Js.Json.t) =>
  result('a, NonEmptyList.t(string));

let oneOf:
  (
    Js.Json.t => result('a, NonEmptyList.t(string)),
    list(Js.Json.t => result('a, NonEmptyList.t(string))),
    Js.Json.t
  ) =>
  result('a, NonEmptyList.t(string));

[@deprecated "Will be removed in favor up the upcoming addition of letops"]
module Pipeline: {
  let succeed: ('a, Js.Json.t) => result('a, NonEmptyList.t(string));

  let pipe:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let field:
    (
      string,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let at:
    (
      list(string),
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let optionalField:
    (
      string,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result(option('a) => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let fallbackField:
    (
      string,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      'a,
      Js.Json.t => result('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let fallback:
    (Js.Json.t => result('a, NonEmptyList.t(string)), 'a, Js.Json.t) =>
    result('a, NonEmptyList.t(string));

  let hardcoded:
    ('a, Js.Json.t => result('a => 'c, NonEmptyList.t(string)), Js.Json.t) =>
    result('c, NonEmptyList.t(string));

  let run:
    (Js.Json.t, Js.Json.t => result('a, NonEmptyList.t(string))) =>
    result('a, NonEmptyList.t(string));

  let map:
    ('a => 'b, Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
    result('b, NonEmptyList.t(string));

  let apply:
    (
      Js.Json.t => result('a => 'b, NonEmptyList.t(string)),
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let map2:
    (
      ('a, 'b) => 'c,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('c, NonEmptyList.t(string));

  let map3:
    (
      ('a, 'b, 'c) => 'd,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('d, NonEmptyList.t(string));

  let map4:
    (
      ('a, 'b, 'c, 'd) => 'e,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t => result('d, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('e, NonEmptyList.t(string));

  let map5:
    (
      ('a, 'b, 'c, 'd, 'e) => 'f,
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t => result('d, NonEmptyList.t(string)),
      Js.Json.t => result('e, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('f, NonEmptyList.t(string));

  let pure: ('a, Js.Json.t) => result('a, NonEmptyList.t(string));

  let flatMap:
    (
      ('a, Js.Json.t) => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));

  let boolean: Js.Json.t => result(bool, NonEmptyList.t(string));
  let string: Js.Json.t => result(Js.String.t, NonEmptyList.t(string));
  let floatFromNumber: Js.Json.t => result(float, NonEmptyList.t(string));
  let intFromNumber: Js.Json.t => result(int, NonEmptyList.t(string));
  let date: Js.Json.t => result(Js.Date.t, NonEmptyList.t(string));
  let variantFromJson:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      'a => option('b),
      Js.Json.t
    ) =>
    result('b, NonEmptyList.t(string));
  let variantFromString:
    (string => option('a), Js.Json.t) => result('a, NonEmptyList.t(string));
  let variantFromInt:
    (int => option('a), Js.Json.t) => result('a, NonEmptyList.t(string));

  let optional:
    (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
    result(option('a), NonEmptyList.t(string));

  let array:
    (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
    result(array('a), NonEmptyList.t(string));
  let list:
    (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
    result(list('a), NonEmptyList.t(string));

  let tuple:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b), NonEmptyList.t(string));

  let tuple2:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b), NonEmptyList.t(string));

  let tuple3:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c), NonEmptyList.t(string));

  let tuple4:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t => result('d, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd), NonEmptyList.t(string));

  let tuple5:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t => result('d, NonEmptyList.t(string)),
      Js.Json.t => result('e, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd, 'e), NonEmptyList.t(string));

  let tupleAtLeast2:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b), NonEmptyList.t(string));

  let tupleAtLeast3:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c), NonEmptyList.t(string));

  let tupleAtLeast4:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t => result('d, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd), NonEmptyList.t(string));

  let tupleAtLeast5:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      Js.Json.t => result('b, NonEmptyList.t(string)),
      Js.Json.t => result('c, NonEmptyList.t(string)),
      Js.Json.t => result('d, NonEmptyList.t(string)),
      Js.Json.t => result('e, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    result(('a, 'b, 'c, 'd, 'e), NonEmptyList.t(string));

  let tupleFromFields:
    (
      (string, Js.Json.t => result('a, NonEmptyList.t(string))),
      (string, Js.Json.t => result('b, NonEmptyList.t(string))),
      Js.Json.t
    ) =>
    result(('a, 'b), NonEmptyList.t(string));

  let dict:
    (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
    result(Js.Dict.t('a), NonEmptyList.t(string));

  let stringMap:
    (Js.Json.t => result('a, NonEmptyList.t(string)), Js.Json.t) =>
    result(Belt.Map.String.t('a), NonEmptyList.t(string));

  let oneOf:
    (
      Js.Json.t => result('a, NonEmptyList.t(string)),
      list(Js.Json.t => result('a, NonEmptyList.t(string))),
      Js.Json.t
    ) =>
    result('a, NonEmptyList.t(string));
};
