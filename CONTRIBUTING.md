### Getting Started

You'll need to already have [Node.js](https://nodejs.org), [Opam](https://opam.ocaml.org/doc/Install.html), and `make`. These instructions also use `yarn` rather than `npm` for working with Node dependencies and scripts (`npm install -g yarn`).

1. Fork [this repo on Github](https://github.com/mlms13/bs-decode) and clone it locally
2. `make init` will configure your environment... this only needs to be done once
3. `make install` will install both `npm` and `opam` dependencies
4. `make build` or `make watch` to build the project and `make test` to run tests
5. Make your changes (and add appropriate tests)
6. [Open a pull request](https://help.github.com/en/articles/about-pull-requests) with your changes

### Code style and conventions

We use [Relude](https://github.com/reazen/relude/) as our standard library, so try to avoid using `Belt` or `Pervasives` when working with lists, options, etc.

To make the code easier to understand for other new contributors, we've tried to minimize our use of infix functions. Using `>>` for forward composition is fine, though.

Also, please make sure that your code has been formatted with [`refmt`](https://reasonml.github.io/docs/en/editor-plugins).

### Project structure

- `Decode_Base.re` is where all of the decoders are defined
- `Decode_As*.re` modules construct `Decode_Base` with everything it needs to produce decoders for a specific output type (e.g. `option`, `result`)
- `Decode_ParseError.re` defines the structured errors and helper functions to work with `result` values of that error type

### Tests

`bs-decode` currently has 100% test coverage, and we hope to keep it that way. :) Running `make test-coverage` will run the tests and give you a coverage report. You can see the detailed report in `./coverage/lcov-report/index.html`, which will help you track down any uncovered functions or branches.

### Documentation

The documentation website is currently generated with [Docusaurus](https://docusaurus.io/). For more information on contributing to, running, and publishing the website, see [the README in the website folder](https://github.com/mlms13/bs-decode/blob/master/website/README.md).

Separately from the website, we maintain `*.rei` interface files that provide type hints and doc comments to editors. When adding new functionality, make sure you update the appropriate interface files with type signatures and comments.
