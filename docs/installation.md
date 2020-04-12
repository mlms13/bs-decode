---
id: installation
title: Installation
---

**Install via npm:**

```sh
npm install --save bs-decode
```

Note that `bs-decode` includes two `peerDependencies`. These are peers instead of regular dependencies to allow you to specify the version in one place so you're less likely to end up with conflicting versions nested in your `node_modules`. If you haven't already, you can install the peer dependencies like this:

```sh
npm install --save relude bs-bastet
```

**Update your bsconfig.json**

```json
"bs-dependencies": [
  "bs-bastet",
  "bs-decode",
  "relude"
],
```
