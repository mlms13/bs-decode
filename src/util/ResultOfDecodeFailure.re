module Result = Belt.Result;
open BsAbstract.Interface;

/**
 * TODO: maybe wrap this whole thing up in a module functor and make the
 * `DecodeBase.failure` a parameter, so that you can easily make this thing
 * with your own custom version of `DecodeBase.failure`, assuming we make that
 * extensible.
 */

type r('a) = Result.t('a, DecodeFailure.t(DecodeBase.failure));

module Functor: FUNCTOR with type t('a) = r('a) = {
  type t('a) = r('a);
  let map = (f, v) => Result.map(v, f);
};

module Apply: APPLY with type t('a) = r('a) = {
  include Functor;
  let apply = (f, v) => switch (f, v) {
  | (Result.Ok(fn), Result.Ok(a)) => Result.Ok(fn(a))
  | (Result.Ok(_), Result.Error(x)) => Result.Error(x)
  | (Result.Error(x), Result.Ok(_)) => Result.Error(x)
  | (Result.Error(fnx), Result.Error(ax)) => Result.Error(DecodeFailure.combine(fnx, ax))
  };
};

module Applicative: APPLICATIVE with type t('a) = r('a) = {
  include Apply;
  let pure = v => Result.Ok(v);
};

module Monad: MONAD with type t('a) = r('a) = {
  include Applicative;
  let flat_map = Result.flatMap;
};

module Alt: ALT with type t('a) = r('a) = {
  include Functor;
  let alt = (a, b) => switch a {
  | Result.Ok(v) => Result.Ok(v)
  | Result.Error(_) => b
  };
};

module Infix = {
  include BsAbstract.Infix.Monad(Monad);
  include BsAbstract.Infix.Alt(Alt);
};

/* Include functions from BsAbstract modules */
/* I could be convinced that this adds noise to the module for little gain */
type t('a) = r('a);
let (map, apply, pure, flat_map) = Monad.(map, apply, pure, flat_map);
let alt = Alt.alt;
include Infix;

/**
 * TODO: this should come for free (as lift2) with BsAbstract.Interface.APPLY
 */
let map2 = (f, a, b) => f <$> a <*> b;

let mapErr = (v, fn) => BsAbstract.Result.Bifunctor.bimap(BsAbstract.Functions.id, v, fn);

/**
 * TODO: this should really come from bs-abstract... waiting on my PR
 */
let note = (failure, opt) => switch opt {
| None => Result.Error(failure)
| Some(v) => Result.Ok(v)
};

let recoverWith = (a) =>
  Alt.alt(_, Applicative.pure(a));
