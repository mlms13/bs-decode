type t('a) =
  | Val('a, Js.Json.t)
  | TriedMultiple(Relude.NonEmpty.List.t(t('a)))
  | Arr(Relude.NonEmpty.List.t((int, t('a))))
  | Obj(Relude.NonEmpty.List.t((string, objError('a))))
and objError('a) =
  | MissingField
  | InvalidField(t('a));

type failure = t(DecodeBase.failure);

let arrPure: (int, t('a)) => t('a);
let objPure: (string, objError('a)) => t('a);
let combine: (t('a), t('a)) => t('a);

let toDebugString:
  (~level: int=?, ~pre: string=?, ('a, Js.Json.t) => string, t('a)) => string;

let failureToDebugString: t(DecodeBase.failure) => string;

module type ValError = {
  type t;
  let handle: DecodeBase.failure => t;
};

module ResultOf:
  (Err: ValError) =>
   {
    module Monad: {
      type nonrec t('a) = Belt.Result.t('a, t(Err.t));
      let map: ('a => 'b, t('a)) => t('b);
      let apply: (t('a => 'b), t('a)) => t('b);
      let pure: 'a => t('a);
      let flat_map: (t('a), 'a => t('b)) => t('b);
    };

    module Alt: {
      type nonrec t('a) = Belt.Result.t('a, t(Err.t));
      let map: ('a => 'b, t('a)) => t('b);
      let alt: (t('a), t('a)) => t('a);
    };

    module TransformError: {
      type nonrec t('a) = Belt.Result.t('a, t(Err.t));
      let valErr: (DecodeBase.failure, Js.Json.t) => t('a);
      let arrErr: (int, t('a)) => t('a);
      let missingFieldErr: string => t('a);
      let objErr: (string, t('a)) => t('a);
      let lazyAlt: (t('a), unit => t('a)) => t('a);
    };
  };
