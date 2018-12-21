module ResultUtil: {
  type r('a) = Belt.Result.t('a, NonEmptyList.t(string));

  let result: ('a => 'b, 'c => 'b, Belt.Result.t('a, 'c)) => 'b;
  let mapErr: ('a => 'b, Belt.Result.t('c, 'a)) => Belt.Result.t('c, 'b);

  module Functor: {
    type t('a) = r('a);
    let map: ('a => 'b, t('a)) => t('b);
  };

  module Apply: {
    type t('a) = r('a);
    let map: ('a => 'b, t('a)) => t('b);
    let apply: (t('a => 'b), t('a)) => t('b);
  };

  module Applicative: {
    type t('a) = r('a);
    let map: ('a => 'b, t('a)) => t('b);
    let apply: (t('a => 'b), t('a)) => t('b);
    let pure: 'a => t('a);
  };

  module Monad: {
    type t('a) = r('a);
    let map: ('a => 'b, t('a)) => t('b);
    let apply: (t('a => 'b), t('a)) => t('b);
    let pure: 'a => t('a);
    let flat_map: (t('a), 'a => t('b)) => t('b);
  };

  module Alt: {
    type t('a) = r('a);
    let map: ('a => 'b, t('a)) => t('b);
    let alt: (t('a), t('a)) => t('a);
  };

  module Infix: {
    let (<*>): (r('a => 'b), r('a)) => r('b);
    let (>>=): (r('a), 'a => r('b)) => r('b);
    let (=<<): ('a => r('b), r('a)) => r('b);
    let (>=>): ('a => r('b), 'b => r('c), 'a) => r('c);
    let (<=<): ('a => r('b), 'c => r('a), 'c) => r('b);
    let (<$>): ('a => 'b, r('a)) => r('b);
    let (<#>): (r('a), 'a => 'b) => r('b);
    let (<|>): (r('a), r('a)) => r('a);
  };

  let note: ('a, option('b)) => Belt.Result.t('b, 'a);
  let recoverWith:
    ('a, Belt.Result.t('a, NonEmptyList.t(string))) =>
    Belt.Result.t('a, NonEmptyList.t(string));
};

let ok: 'a => Belt.Result.t('a, NonEmptyList.t(string));

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
    (Js.Dict.key, Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))),
    (Js.Dict.key, Js.Json.t => Belt.Result.t('b, NonEmptyList.t(string))),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), NonEmptyList.t(string));

let dict:
  (Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)), Js.Json.t) =>
  Belt.Result.t(Js.Dict.t('a), NonEmptyList.t(string));

let at:
  (
    list(Js.Dict.key),
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let field:
  (
    Js.Dict.key,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let optionalField:
  (
    Js.Dict.key,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    Js.Json.t
  ) =>
  Belt.Result.t(option('a), NonEmptyList.t(string));

let fallback:
  (
    Js.Dict.key,
    Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
    'a,
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

let oneOf:
  (
    NonEmptyList.t(Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))),
    Js.Json.t
  ) =>
  Belt.Result.t('a, NonEmptyList.t(string));

module Pipeline: {
  let succeed: ('a, 'b) => Belt.Result.t('a, NonEmptyList.t(string));

  let map2:
    (
      ('a, 'b) => 'c,
      'd => Belt.Result.t('a, NonEmptyList.t(string)),
      'd => Belt.Result.t('b, NonEmptyList.t(string)),
      'd
    ) =>
    Belt.Result.t('c, NonEmptyList.t(string));

  let map3:
    (
      ('a, 'b, 'c) => 'd,
      'e => Belt.Result.t('a, NonEmptyList.t(string)),
      'e => Belt.Result.t('b, NonEmptyList.t(string)),
      'e => Belt.Result.t('c, NonEmptyList.t(string)),
      'e
    ) =>
    Belt.Result.t('d, NonEmptyList.t(string));

  let map4:
    (
      ('a, 'b, 'c, 'd) => 'e,
      'f => Belt.Result.t('a, NonEmptyList.t(string)),
      'f => Belt.Result.t('b, NonEmptyList.t(string)),
      'f => Belt.Result.t('c, NonEmptyList.t(string)),
      'f => Belt.Result.t('d, NonEmptyList.t(string)),
      'f
    ) =>
    Belt.Result.t('e, NonEmptyList.t(string));

  let map5:
    (
      ('a, 'b, 'c, 'd, 'e) => 'f,
      'g => Belt.Result.t('a, NonEmptyList.t(string)),
      'g => Belt.Result.t('b, NonEmptyList.t(string)),
      'g => Belt.Result.t('c, NonEmptyList.t(string)),
      'g => Belt.Result.t('d, NonEmptyList.t(string)),
      'g => Belt.Result.t('e, NonEmptyList.t(string)),
      'g
    ) =>
    Belt.Result.t('f, NonEmptyList.t(string));

  let field:
    (
      Js.Dict.key,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let at:
    (
      list(Js.Dict.key),
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let optionalField:
    (
      Js.Dict.key,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      Js.Json.t => Belt.Result.t(option('a) => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let fallback:
    (
      Js.Dict.key,
      Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string)),
      'a,
      Js.Json.t => Belt.Result.t('a => 'b, NonEmptyList.t(string)),
      Js.Json.t
    ) =>
    Belt.Result.t('b, NonEmptyList.t(string));

  let hardcoded:
    ('a, 'b => Belt.Result.t('a => 'c, NonEmptyList.t(string)), 'b) =>
    Belt.Result.t('c, NonEmptyList.t(string));

  let run:
    (Js.Json.t, Js.Json.t => Belt.Result.t('a, NonEmptyList.t(string))) =>
    Belt.Result.t('a, NonEmptyList.t(string));
};
