type t('a) =
  | Val('a, Js.Json.t)
  | Arr(NonEmptyList.t((int, t('a))))
  | Obj(NonEmptyList.t((string, objError('a))))

and objError('a) =
  | MissingField
  | InvalidField(t('a));


let objPure = (field, err) =>
  Obj(NonEmptyList.pure((field, err)));

/*
 * This is almost like Semigroup's `append`, but associativity only holds when
 * both errors are `Arr(...)` or `Obj(...)`. In practice this is fine, because
 * those are the only times when you actually want to combine errors.
 */
let combine = (a, b) => switch (a, b) {
| (Arr(nela), Arr(nelb)) => Arr(NonEmptyList.append(nela, nelb))
| (Obj(nela), Obj(nelb)) => Obj(NonEmptyList.append(nela, nelb))
| _ => a
};

/**
 * Traverse the tree of errors and produce properly-indented error strings:
 *
 * Failed to decode array:
 *     At position 0: Expected string but found 3.5
 *
 * Failed to decode array:
 *     At position 3: Failed to decode object:
 *         Field "foo" is required but was not present
 *         Field "bar" had an invalid value: Failed to decode array:
 *             At position 0: Expected int but found []
 */
let rec toDebugString = (~level=0, ~pre="", innerToString, v) => {
  let spaces = indent => String.make(indent * 4, ' ');

  let msg = switch v {
  | Val(x, json) => innerToString(x, json)
  | Arr(nel) =>
    let childMessages = nel |> NonEmptyList.map(((i, err)) =>
      toDebugString(~level = level + 1, ~pre = "At position " ++ string_of_int(i) ++ ": ", innerToString, err)
    ) |> NonEmptyList.toT |> Array.of_list |> Js.Array.joinWith("\n");

    "Failed to decode array:\n" ++ childMessages;

  | Obj(nel) =>
    let childMessages = nel |> NonEmptyList.map(((field, err)) => {
      let fieldStr = "\"" ++ field ++ "\"";
      switch err {
      | MissingField => spaces(level + 1) ++ "Field " ++ fieldStr ++ " is required, but was not present"
      | InvalidField(err) => toDebugString(~level = level + 1, ~pre = "Field " ++ fieldStr ++ " had an invalid value: ", innerToString, err)
      };
    }) |> NonEmptyList.toT |> Array.of_list |> Js.Array.joinWith("\n");

    "Failed to decode object:\n" ++ childMessages;
  };

  spaces(level) ++ pre ++ msg;
};

/* let rec toNelString = v => switch v {
| Val(x, json) =>
  NonEmptyList.pure(valErrorToString(x, json))

| Arr(nel) =>
  NonEmptyList.flat_map(nel, ((pos, err)) =>
    toNelString(err) |> NonEmptyList.map(x => "While decoding array at position " ++ string_of_int(pos) ++ ": " ++ x)
  )

| Obj(nel) =>
  NonEmptyList.flat_map(nel, ((field, err)) => switch err {
  | MissingField => NonEmptyList.pure("Object field \"" ++ field ++ "\" was missing")
  | InvalidField(x) => toNelString(x) |> NonEmptyList.map(x => "While decoding field \"" ++ field ++ "\": " ++ x)
  })
}; */
