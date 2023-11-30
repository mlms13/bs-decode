(lang dune 3.8)

(using directory-targets 0.1)

(using melange 0.1)

(source
 (github mlms13/bs-decode))

(license MIT)

(maintainers "Michael Martin")

(authors "Michael Martin")

(homepage https://github.com/mlms13/bs-decode)

(bug_reports https://github.com/mlms13/bs-decode/issues)

(package
 (name bs-decode)
 (synopsis "Type-safe JSON decoding for ReasonML and OCaml")
 (description "Type-safe JSON decoding for ReasonML and OCaml")
 (depends
  dot-merlin-reader
  (melange
   (>= 2.0.0))
  (ocaml
   (>= 5.1.0))
  (reason
   (>= 3.10.0))
  ocaml-lsp-server
  relude
  bastet
  (melange-jest
   (>= 0.1.0))
  opam-check-npm-deps))

(generate_opam_files)
