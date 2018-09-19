module OptionTransform: DecodeBase.TransformError with type t('a) = option('a) = {
  type t('a) = option('a);
  let transform = _ => None;
  let mapErr = (_fn, v) => switch v {
  | Some(a) => Some(a)
  | None => None
  };
  let arrErr = mapErr;
  let objErr = mapErr;
};

module DecodeAsOption =
  DecodeBase.DecodeBase(
    OptionTransform,
    BsAbstract.Option.Monad,
    BsAbstract.Option.Alt
  );

include DecodeAsOption;
