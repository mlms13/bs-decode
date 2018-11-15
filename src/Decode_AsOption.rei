let ok: 'a => option('a);

let boolean: Js.Json.t => option(bool);
let string: Js.Json.t => option(Js.String.t);
let float: Js.Json.t => option(float);
let int: Js.Json.t => option(int);

let optional: (Js.Json.t => option('a), Js.Json.t) => option(option('a));

let array: (Js.Json.t => option('a), Js.Json.t) => option(array('a));
let list: (Js.Json.t => option('a), Js.Json.t) => option(list('a));
let tuple:
  (
    (Js.Dict.key, Js.Json.t => option('a)),
    (Js.Dict.key, Js.Json.t => option('b)),
    Js.Json.t
  ) =>
  option(('a, 'b));

let dict: (Js.Json.t => option('a), Js.Json.t) => option(Js.Dict.t('a));

let at:
  (list(Js.Dict.key), Js.Json.t => option('a), Js.Json.t) => option('a);

let field: (Js.Dict.key, Js.Json.t => option('a), Js.Json.t) => option('a);

let optionalField:
  (Js.Dict.key, Js.Json.t => option('a), Js.Json.t) => option(option('a));

let fallback:
  (Js.Dict.key, Js.Json.t => option('a), 'a, Js.Json.t) => option('a);

let oneOf:
  (NonEmptyList.t(Js.Json.t => option('a)), Js.Json.t) => option('a);

module Pipeline: {
  let succeed: ('a, 'b) => option('a);

  let map2:
    (('a, 'b) => 'c, 'd => option('a), 'd => option('b), 'd) => option('c);

  let map3:
    (
      ('a, 'b, 'c) => 'd,
      'e => option('a),
      'e => option('b),
      'e => option('c),
      'e
    ) =>
    option('d);

  let map4:
    (
      ('a, 'b, 'c, 'd) => 'e,
      'f => option('a),
      'f => option('b),
      'f => option('c),
      'f => option('d),
      'f
    ) =>
    option('e);

  let map5:
    (
      ('a, 'b, 'c, 'd, 'e) => 'f,
      'g => option('a),
      'g => option('b),
      'g => option('c),
      'g => option('d),
      'g => option('e),
      'g
    ) =>
    option('f);

  let field:
    (
      Js.Dict.key,
      Js.Json.t => option('a),
      Js.Json.t => option('a => 'b),
      Js.Json.t
    ) =>
    option('b);

  let optionalField:
    (
      Js.Dict.key,
      Js.Json.t => option('a),
      Js.Json.t => option(option('a) => 'b),
      Js.Json.t
    ) =>
    option('b);

  let fallback:
    (
      Js.Dict.key,
      Js.Json.t => option('a),
      'a,
      Js.Json.t => option('a => 'b),
      Js.Json.t
    ) =>
    option('b);

  let hardcoded: ('a, 'b => option('a => 'c), 'b) => option('c);

  let run: (Js.Json.t, Js.Json.t => option('a)) => option('a);
};
