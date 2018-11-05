module OptionTransform:
  DecodeBase.TransformError with type t('a) = option('a) = {
  type t('a) = option('a);
  let valErr = (_, _) => None;
  let arrErr = (_, opt) => opt;
  let missingFieldErr = _ => None;
  let objErr = (_, opt) => opt;
  let lazyAlt = (opt, fn) =>
    switch (opt) {
    | Some(v) => Some(v)
    | None => fn()
    };
};

module DecodeAsOption =
  DecodeBase.DecodeBase(
    OptionTransform,
    BsAbstract.Option.Monad,
    BsAbstract.Option.Alt,
  );

include DecodeAsOption;
