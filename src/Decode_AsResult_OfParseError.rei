module NonEmptyList: Decode_NonEmptyList.Nel;

let map2:
  (
    ('a, 'b) => 'c,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('b, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('c, Decode_ParseError.failure);

let map3:
  (
    ('a, 'b, 'c) => 'd,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('b, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('c, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('d, Decode_ParseError.failure);

let map4:
  (
    ('a, 'b, 'c, 'd) => 'e,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('b, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('c, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('d, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('e, Decode_ParseError.failure);

let map5:
  (
    ('a, 'b, 'c, 'd, 'e) => 'f,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('b, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('c, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('d, Decode_ParseError.failure),
    Js.Json.t => Belt.Result.t('e, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('f, Decode_ParseError.failure);

module ResultUtil: {
  type r('a) = Belt.Result.t('a, Decode_ParseError.failure);

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
    ('a, Belt.Result.t('a, Decode_ParseError.failure)) =>
    Belt.Result.t('a, Decode_ParseError.failure);
};

let boolean: Js.Json.t => Belt.Result.t(bool, Decode_ParseError.failure);
let string: Js.Json.t => Belt.Result.t(string, Decode_ParseError.failure);
[@ocaml.deprecated "Use floatFromNumber instead."]
let float: Js.Json.t => Belt.Result.t(float, Decode_ParseError.failure);
[@ocaml.deprecated "Use intFromNumber instead."]
let int: Js.Json.t => Belt.Result.t(int, Decode_ParseError.failure);
let floatFromNumber:
  Js.Json.t => Belt.Result.t(float, Decode_ParseError.failure);
let intFromNumber: Js.Json.t => Belt.Result.t(int, Decode_ParseError.failure);
let date: Js.Json.t => Belt.Result.t(Js.Date.t, Decode_ParseError.failure);
let variantFromJson:
  (
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    'a => option('b),
    Js.Json.t
  ) =>
  Belt.Result.t('b, Decode_ParseError.failure);
let variantFromString:
  (string => option('a), Js.Json.t) =>
  Belt.Result.t('a, Decode_ParseError.failure);
let variantFromInt:
  (int => option('a), Js.Json.t) =>
  Belt.Result.t('a, Decode_ParseError.failure);

let optional:
  (Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure), Js.Json.t) =>
  Belt.Result.t(option('a), Decode_ParseError.failure);

let array:
  (Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure), Js.Json.t) =>
  Belt.Result.t(array('a), Decode_ParseError.failure);

let list:
  (Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure), Js.Json.t) =>
  Belt.Result.t(list('a), Decode_ParseError.failure);

let tuple:
  (
    (Js.Dict.key, Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure)),
    (Js.Dict.key, Js.Json.t => Belt.Result.t('b, Decode_ParseError.failure)),
    Js.Json.t
  ) =>
  Belt.Result.t(('a, 'b), Decode_ParseError.failure);

let dict:
  (Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure), Js.Json.t) =>
  Belt.Result.t(Js.Dict.t('a), Decode_ParseError.failure);

let at:
  (
    list(Js.Dict.key),
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('a, Decode_ParseError.failure);

let field:
  (
    Js.Dict.key,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t('a, Decode_ParseError.failure);

let optionalField:
  (
    Js.Dict.key,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    Js.Json.t
  ) =>
  Belt.Result.t(option('a), Decode_ParseError.failure);

let fallback:
  (
    Js.Dict.key,
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    'a,
    Js.Json.t
  ) =>
  Belt.Result.t('a, Decode_ParseError.failure);

let oneOf:
  (
    Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
    list(Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure)),
    Js.Json.t
  ) =>
  Belt.Result.t('a, Decode_ParseError.failure);

module Pipeline: {
  let succeed: ('a, 'b) => Belt.Result.t('a, Decode_ParseError.failure);

  let field:
    (
      Js.Dict.key,
      Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
      Js.Json.t => Belt.Result.t('a => 'b, Decode_ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, Decode_ParseError.failure);

  let at:
    (
      list(Js.Dict.key),
      Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
      Js.Json.t => Belt.Result.t('a => 'b, Decode_ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, Decode_ParseError.failure);

  let optionalField:
    (
      Js.Dict.key,
      Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
      Js.Json.t => Belt.Result.t(option('a) => 'b, Decode_ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, Decode_ParseError.failure);

  let fallback:
    (
      Js.Dict.key,
      Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure),
      'a,
      Js.Json.t => Belt.Result.t('a => 'b, Decode_ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('b, Decode_ParseError.failure);

  let hardcoded:
    (
      'a,
      Js.Json.t => Belt.Result.t('a => 'c, Decode_ParseError.failure),
      Js.Json.t
    ) =>
    Belt.Result.t('c, Decode_ParseError.failure);

  let run:
    (Js.Json.t, Js.Json.t => Belt.Result.t('a, Decode_ParseError.failure)) =>
    Belt.Result.t('a, Decode_ParseError.failure);
};
