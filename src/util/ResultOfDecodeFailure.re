open BsAbstract.Interface;

type t('a) = Belt.Result.t('a, DecodeFailure.t);

let map = (f, v) =>
  Belt.Result.map(v, f);

let ap = (f, v) => switch (f, v) {
| (Belt.Result.Ok(fn), Belt.Result.Ok(a)) => Belt.Result.Ok(fn(a))
| (Belt.Result.Ok(_), Belt.Result.Error(x)) => Belt.Result.Error(x)
| (Belt.Result.Error(x), Belt.Result.Ok(_)) => Belt.Result.Error(x)
| (Belt.Result.Error(fnx), Belt.Result.Error(ax)) => Belt.Result.Error(DecodeFailure.combine(fnx, ax))
};

let pure = v => Belt_Result.Ok(v);

let flat_map = (f, a) => switch a {
| Belt.Result.Error(x) => Belt.Result.Error(x)
| Belt.Result.Ok(v) => f(v)
};

let alt = (a, b) => switch a {
| Belt.Result.Error(_) => b
| Belt.Result.Ok(v) => Belt.Result.Ok(v)
};

module Functor: FUNCTOR with type t('a) = t('a) = {
  type t('a) = Belt.Result.t('a, DecodeFailure.t);
  let map = map;
};

module Apply: APPLY with type t('a) = t('a) = {
  include Functor;
  let apply = ap;
};

module Applicative: APPLICATIVE with type t('a) = t('a) = {
  include Apply;
  let pure = pure;
};

module Monad: MONAD with type t('a) = t('a) = {
  include Applicative;
  let flat_map = (a, f) => flat_map(f, a);
};

module Alt: ALT with type t('a) = t('a) = {
  include Functor;
  let alt = alt;
};

module InfixApply = BsAbstract.Infix.Apply(Apply);

let ((<$>), (<*>)) = InfixApply.((<$>), (<*>));
let map2 = (f, a, b) => f <$> a <*> b;

module InfixAlt = BsAbstract.Infix.Alt(Alt);
let (<|>) = InfixAlt.(<|>);

let mapErr = (fn, v) => BsAbstract.Result.Bifunctor.bimap(BsAbstract.Functions.id, fn, v);

/**
 * TODO: this should really come from bs-abstract... waiting on my PR
 */
let note = (failure, opt) => switch opt {
| None => Belt.Result.Error(failure)
| Some(v) => Belt.Result.Ok(v)
};

let recoverWith = (a) =>
  alt(_, pure(a));
