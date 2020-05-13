open Relude.Globals;

module Wat = {
  type monad('a) = option('a);

  type t('a) =
    | Pure('a): t('a)
    | Apply(monad('x), t('x => 'a)): t('a);
};

module ParseError = Decode_ParseError;

module type Input = {
  type t;
  let bool: t => option(bool);
  let float: t => option(float);
  let string: t => option(string);
  let array: t => option(array(t));
  let object_: t => option(array((string, t)));
  let optional: t => option(unit);
};

module Make = (Input: Input) => {
  type t('a) = Input.t => result('a, ParseError.base);
  let map = (f, decode, inpt) => decode(inpt) |> Result.map(f);
};

module Json: Input = {
  type t = Js.Json.t;
  let bool = Js.Json.decodeBoolean;
  let float = Js.Json.decodeNumber;
  let string = Js.Json.decodeString;
  let array = Js.Json.decodeArray;
  let object_ = Js.Json.decodeObject >> Option.map(Js.Dict.entries);
  let optional = Js.Json.decodeNull >> Option.map(_ => ());
};

module Schema = {
  include Free.Applicative.WithFunctor((Make(Json)));
};
