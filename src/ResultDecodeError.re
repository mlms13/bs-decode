open BsAbstract.Interface;

type t('a) = Belt.Result.t('a, DecodeError.t);

let map = (f, v) =>
  Belt.Result.map(v, f);

let ap = (f, v) => switch (f, v) {
| (Belt.Result.Ok(fn), Belt.Result.Ok(a)) => Belt.Result.Ok(fn(a))
| (Belt.Result.Ok(_), Belt.Result.Error(x)) => Belt.Result.Error(x)
| (Belt.Result.Error(x), Belt.Result.Ok(_)) => Belt.Result.Error(x)
| (Belt.Result.Error(fnx), Belt.Result.Error(ax)) => Belt.Result.Error(DecodeError.combine(fnx, ax))
};

let alt = (a, b) => switch a {
| Belt.Result.Error(_) => b
| Belt.Result.Ok(v) => Belt.Result.Ok(v)
};

module Functor: FUNCTOR with type t('a) = t('a) = {
  type t('a) = Belt.Result.t('a, DecodeError.t);
  let map = map;
};

module Apply: APPLY with type t('a) = t('a) = {
  include Functor;
  let apply = ap;
};

module InfixApply = BsAbstract.Infix.Apply(Apply);

let ((<$>), (<*>)) = InfixApply.((<$>), (<*>));

/* let map2 = (f, a, b) => ap(map(f, a), b); */
let map2 = (f, a, b) => f <$> a <*> b;

module Alt: ALT with type t('a) = t('a) = {
  include Functor;
  let alt = alt;
};

let mapErr = (fn, v) => BsAbstract.Result.Bifunctor.bimap(BsAbstract.Functions.id, fn, v);
