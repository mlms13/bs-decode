### Getting Started

1. Fork [this repo on Github](https://github.com/mlms13/bs-decode) and clone it locally
2. `npm install` to grab dependencies
3. `npm run cleanbuild && npm run test` to build and run tests
4. Make your changes (and add appropriate tests)
5. [Open a pull request](https://help.github.com/en/articles/about-pull-requests) with your changes

### Code style and conventions

We use [Relude](https://github.com/reazen/relude/) as our standard library, so try to avoid using `Belt` or `Pervasives` when working with lists, options, etc.

To make the code easier to understand for other new contributors, we've tried to minimize our use of infix functions. Using `>>` for forward composition is fine, though.

Also, please make sure that your code has been formatted with [`refmt`](https://reasonml.github.io/docs/en/editor-plugins).

### Project structure

- `Decode_Base.re` is where all of the decoders are defined
- `Decode_As*.re` modules construct `Decode_Base` with everything it needs to produce decoders for a specific output type (e.g. `option`, `result`)
- `Decode_ParseError.re` defines the structured errors and helper functions to work with `result` values of that error type

### Tests

`bs-decode` currently has 100% test coverage, and we hope to keep it that way. :) Running `npm run test` (or `jest --coverage`) will run your tests and give you a coverage report. You can see the detailed report in `./coverage/lcov-report/index.html`, which will help you track down any uncovered functions or branches.

### Documentation

The documentation website is currently generated with [Docusaurus](https://docusaurus.io/). For more information on contributing to, running, and publishing the website, see [the README in the website folder](https://github.com/mlms13/bs-decode/blob/master/website/README.md).

Separately from the website, we maintain `*.rei` interface files that provide type hints and doc comments to editors. When adding new functionality, make sure you update the appropriate interface files with type signatures and comments.
