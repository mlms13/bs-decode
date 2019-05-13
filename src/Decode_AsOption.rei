let map: ('a => 'b, Js.Json.t => option('a), Js.Json.t) => option('b);
let apply:
  (Js.Json.t => option('a => 'b), Js.Json.t => option('a), Js.Json.t) =>
  option('b);

let map2:
  (
    ('a, 'b) => 'c,
    Js.Json.t => option('a),
    Js.Json.t => option('b),
    Js.Json.t
  ) =>
  option('c);

let map3:
  (
    ('a, 'b, 'c) => 'd,
    Js.Json.t => option('a),
    Js.Json.t => option('b),
    Js.Json.t => option('c),
    Js.Json.t
  ) =>
  option('d);

let map4:
  (
    ('a, 'b, 'c, 'd) => 'e,
    Js.Json.t => option('a),
    Js.Json.t => option('b),
    Js.Json.t => option('c),
    Js.Json.t => option('d),
    Js.Json.t
  ) =>
  option('e);

let map5:
  (
    ('a, 'b, 'c, 'd, 'e) => 'f,
    Js.Json.t => option('a),
    Js.Json.t => option('b),
    Js.Json.t => option('c),
    Js.Json.t => option('d),
    Js.Json.t => option('e),
    Js.Json.t
  ) =>
  option('f);

let pure: ('a, Js.Json.t) => option('a);

let flatMap:
  (('a, Js.Json.t) => option('b), Js.Json.t => option('a), Js.Json.t) =>
  option('b);

let boolean: Js.Json.t => option(bool);
let string: Js.Json.t => option(Js.String.t);
[@ocaml.deprecated "Use floatFromNumber instead."]
let float: Js.Json.t => option(float);
[@ocaml.deprecated "Use intFromNumber instead."]
let int: Js.Json.t => option(int);
let floatFromNumber: Js.Json.t => option(float);
let intFromNumber: Js.Json.t => option(int);
let date: Js.Json.t => option(Js.Date.t);
let variantFromJson:
  (Js.Json.t => option('a), 'a => option('b), Js.Json.t) => option('b);
let variantFromString: (string => option('a), Js.Json.t) => option('a);
let variantFromInt: (int => option('a), Js.Json.t) => option('a);

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
  (Js.Json.t => option('a), list(Js.Json.t => option('a)), Js.Json.t) =>
  option('a);

module Pipeline: {
  let succeed: ('a, 'b) => option('a);

  let field:
    (
      Js.Dict.key,
      Js.Json.t => option('a),
      Js.Json.t => option('a => 'b),
      Js.Json.t
    ) =>
    option('b);

  let at:
    (
      list(Js.Dict.key),
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

  let hardcoded:
    ('a, Js.Json.t => option('a => 'c), Js.Json.t) => option('c);

  let run: (Js.Json.t, Js.Json.t => option('a)) => option('a);
};
