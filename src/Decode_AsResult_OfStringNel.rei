module NonEmptyList: Decode_NonEmptyList.Nel;

let map:
  (
    'a => 'b,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('b, NonEmptyList.t(string));

let apply:
  (
    Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('b, NonEmptyList.t(string));

let map2:
  (
    ('a, 'b) => 'c,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('c, NonEmptyList.t(string));

let map3:
  (
    ('a, 'b, 'c) => 'd,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('c, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('d, NonEmptyList.t(string));

let map4:
  (
    ('a, 'b, 'c, 'd) => 'e,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('c, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('d, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('e, NonEmptyList.t(string));

let map5:
  (
    ('a, 'b, 'c, 'd, 'e) => 'f,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('c, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('d, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('e, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('f, NonEmptyList.t(string));

let pure: ('a, Js.Json.t) => Belt.Result.t('a, NonEmptyList.t(string));

let flatMap:
  (
    ('a, Js.Json.t) => Belt.Result.t('b, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('b, NonEmptyList.t(string));

let alt:
  (
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let boolean: Js.Json.t => Belt.Result.t(bool, NonEmptyList.t(string));
let string: Js.Json.t => Belt.Result.t(string, NonEmptyList.t(string));
[@ocaml.deprecated "Use floatFromNumber instead."]
let float: Js.Json.t => Belt.Result.t(float, NonEmptyList.t(string));
[@ocaml.deprecated "Use intFromNumber instead."]
let int: Js.Json.t => Belt.Result.t(int, NonEmptyList.t(string));
let floatFromNumber:
  Js.Json.t => Belt.Result.t(float, NonEmptyList.t(string));
let intFromNumber: Js.Json.t => Belt.Result.t(int, NonEmptyList.t(string));
let date: Js.Json.t => Belt.Result.t(Js.Date.t, NonEmptyList.t(string));
let variantFromJson:
  (
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    'a => option('b),
    Js.Json.t
  ) =>
  Belt.Result.t('b, NonEmptyList.t(string));
let variantFromString:
  (string => option('a), Js.Json.t) =>
  Belt.Result.t('a, NonEmptyList.t(string));
let variantFromInt:
  (int => option('a), Js.Json.t) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let optional:
  (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
  Belt.Result.t(option('a), NonEmptyList.t(string));

let array:
  (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
  Belt.Result.t(array('a), NonEmptyList.t(string));

let list:
  (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
  Belt.Result.t(list('a), NonEmptyList.t(string));

let tuple:
  (
    (string, Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))),
    (string, Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string))),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), NonEmptyList.t(string));

let dict:
  (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
  Belt.Result.t(Js.Dict.t('a), NonEmptyList.t(string));

let at:
  (
    list(string),
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let field:
  (
    string,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let optionalField:
  (
    string,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t(option('a), NonEmptyList.t(string));

let fallback:
  (
    string,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    'a,
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let oneOf:
  (
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    list(Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

module Pipeline: {
  let succeed: ('a, Js.Json.t) => Belt.Result.t('a, NonEmptyList.t(string));

  let field:
    (
      string,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let at:
    (
      list(string),
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let optionalField:
    (
      string,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t(option('a) => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let fallback:
    (
      string,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      'a,
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let hardcoded:
    (
      'a,
      Js.Json.t => Belt.Result.t('a => 'c, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('c, NonEmptyList.t(string));

  let run:
    (Js.Json.t, Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))) =>
    Belt.Result.t('a, NonEmptyList.t(string));

  let map:
    (
      'a => 'b,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let apply:
    (
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let map2:
    (
      ('a, 'b) => 'c,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('c, NonEmptyList.t(string));

  let map3:
    (
      ('a, 'b, 'c) => 'd,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('c, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('d, NonEmptyList.t(string));

  let map4:
    (
      ('a, 'b, 'c, 'd) => 'e,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('c, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('d, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('e, NonEmptyList.t(string));

  let map5:
    (
      ('a, 'b, 'c, 'd, 'e) => 'f,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('c, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('d, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('e, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('f, NonEmptyList.t(string));

  let pure: ('a, Js.Json.t) => Belt.Result.t('a, NonEmptyList.t(string));

  let flatMap:
    (
      ('a, Js.Json.t) => Belt.Result.t('b, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let boolean: Js.Json.t => Belt.Result.t(bool, NonEmptyList.t(string));
  let string:
    Js.Json.t => Belt.Result.t(Js.String.t, NonEmptyList.t(string));
  let floatFromNumber:
    Js.Json.t => Belt.Result.t(float, NonEmptyList.t(string));
  let intFromNumber: Js.Json.t => Belt.Result.t(int, NonEmptyList.t(string));
  let date: Js.Json.t => Belt.Result.t(Js.Date.t, NonEmptyList.t(string));
  let variantFromJson:
    (
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      'a => option('b),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));
  let variantFromString:
    (string => option('a), Js.Json.t) =>
    Belt.Result.t('a, NonEmptyList.t(string));
  let variantFromInt:
    (int => option('a), Js.Json.t) =>
    Belt.Result.t('a, NonEmptyList.t(string));

  let optional:
    (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
    Belt.Result.t(option('a), NonEmptyList.t(string));

  let array:
    (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
    Belt.Result.t(array('a), NonEmptyList.t(string));
  let list:
    (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
    Belt.Result.t(list('a), NonEmptyList.t(string));

  let tuple:
    (
      (string, Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))),
      (string, Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string))),
      Js.Json.t
    ) =>
    Belt.Result.t(('a, 'b), NonEmptyList.t(string));

  let dict:
    (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
    Belt.Result.t(Js.Dict.t('a), NonEmptyList.t(string));

  let oneOf:
    (
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      list(Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))),
      Js.Json.t
    ) =>
    Belt.Result.t('a, NonEmptyList.t(string));
};
