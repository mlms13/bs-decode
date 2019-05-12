/**
 * We re-export some of Relude.NonEmptyList to make it easier for downstream
 * projects to work with the output of `bs-decode` without explicitly adding
 * a dependency on Relude.
 *
 * Specifically, we add a module type, so that specific kinds of decoders can
 * include this definition of `NonEmptyList` in their rei files.
 */
module type Nel = {
  type t('a) = Relude.NonEmpty.List.t('a);
  let make: ('a, list('a)) => t('a);
  let pure: 'a => t('a);
  let cons: ('a, t('a)) => t('a);
  let map: ('a => 'b, t('a)) => t('b);
  let foldLeft: (('b, 'a) => 'b, 'b, t('a)) => 'b;
  let head: t('a) => 'a;
  let tail: t('a) => list('a);
  let toSequence: t('a) => list('a);
};
