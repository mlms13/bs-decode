type base = [
  | `ExpectedNull
  | `ExpectedBoolean
  | `ExpectedString
  | `ExpectedNumber
  | `ExpectedInt
  | `ExpectedArray
  | `ExpectedTuple(int)
  | `ExpectedObject
  | `ExpectedValidDate
  | `ExpectedValidOption
];

type t('a) =
  | Val('a, Js.Json.t)
  | TriedMultiple(Relude.NonEmpty.List.t(t('a)))
  | Arr(Relude.NonEmpty.List.t((int, t('a))))
  | Obj(Relude.NonEmpty.List.t((string, objError('a))))
and objError('a) =
  | MissingField
  | InvalidField(t('a));

type failure = t(base);

[@deprecated "Extending the result type is deprecated"]
module type TransformError = {
  type t('a);
  let valErr: (base, Js.Json.t) => t('a);
  let arrErr: (int, t('a)) => t('a);
  let missingFieldErr: string => t('a);
  let objErr: (string, t('a)) => t('a);
  let lazyAlt: (t('a), unit => t('a)) => t('a);
};

let arrPure: (int, t('a)) => t('a);
let objPure: (string, objError('a)) => t('a);
let combine: (t('a), t('a)) => t('a);

let failureToString: (base, Js.Json.t) => string;

let toDebugString:
  (~level: int=?, ~pre: string=?, ('a, Js.Json.t) => string, t('a)) => string;

let failureToDebugString: t(base) => string;

[@deprecated "Extending the result type is deprecated"]
module type ValError = {
  type t;
  let handle: base => t;
};

[@deprecated "Extending the result type is deprecated"]
[@ocaml.warning "-3"]
module ResultOf:
  (Err: ValError) =>
   {
    type error = t(Err.t);
    type nonrec t('a) = result('a, error);
    let map: ('a => 'b, t('a)) => t('b);
    let apply: (t('a => 'b), t('a)) => t('b);
    let pure: 'a => t('a);
    let flat_map: (t('a), 'a => t('b)) => t('b);

    module TransformError: {
      type nonrec t('a) = result('a, error);
      let valErr: (base, Js.Json.t) => t('a);
      let arrErr: (int, t('a)) => t('a);
      let missingFieldErr: string => t('a);
      let objErr: (string, t('a)) => t('a);
      let lazyAlt: (t('a), unit => t('a)) => t('a);
    };
  };
