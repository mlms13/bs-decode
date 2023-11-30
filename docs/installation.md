---
id: installation
title: Installation
---

**Install:**

```sh
yarn add bs-decode
```

Note that `bs-decode` includes two `peerDependencies`. These are peers instead of regular dependencies to allow you to specify the version in one place so you're less likely to end up with conflicting versions nested in your `node_modules`. If you haven't already, you can install the peer dependencies like this:

```sh
yarn add bs-decode relude bs-bastet
```

**Add the library stanzas in your dune file**

If you're using [dune](https://dune.build/) to build your project, add a [library stanza](https://dune.readthedocs.io/en/stable/dune-files.html#library) to tell dune about `bs-decode`. You'll need to add similar library stanzas for `bsBastet` and `relude` if you haven't already.

```
(subdir
 node_modules/bs-decode/src
 (library
  (name bsDecode)
  (modes melange)
  (libraries bsBastet relude)))
```
