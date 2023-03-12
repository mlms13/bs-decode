(lang dune 3.7)

(using directory-targets 0.1)

(using melange 0.1)

(generate_opam_files true)

(name bs-decode)

(license "MIT")

(maintainers Michael Martin)

(authors Michael Martin)

(homepage https://github.com/mlms13/bs-decode)

(bug_reports https://github.com/mlms13/bs-decode/issues)

(package
 (allow_empty)
 (name bs-decode)
 (synopsis "Type-safe JSON decoding for ReasonML and OCaml")
 (description "Type-safe JSON decoding for ReasonML and OCaml")
 (depends
  (ocaml
   (and
    (>= "4.14.0")
    (< "4.15.0")))
  (reason
   (and
    (>= "3.8.2")
    (< "4.0.0")))
  mel
  melange
  (ocaml-lsp-server :dev)
  (dot-merlin-reader :dev)))
