## src/include — Public API headers

Top-level headers for the ccjieba library, under `namespace ccjieba`. `jieba.hh` is the aggregate entry point; most consumers only need `#include "jieba.hh"`.

### Files

- **jieba.hh** — `Jieba` struct with `cut()`, `extract()`, `tag_word()`, `tag_sentence()` methods.
- **keyword.hh** — `Keyword` struct (weight, word, offsets) returned by `extract()`.
- **index/hmmodel.hh** — Hidden Markov Model for POS tagging (states: B/E/M/S).
- **index/idf.hh** — Inverse Document Frequency index for keyword extraction scoring.
- **index/stop_words.hh** — Stop-words dictionary; `operator[]` returns `bool`.
- **index/trie.hh** — Trie data structure backing dictionary lookup and user-defined dictionaries.
- **utils/io.hh** — Binary stream I/O wrappers (`bostream`, `bistream`) for serialization.

### Conventions

- All headers use `#pragma once`. Trailing-return-type syntax (`auto foo() -> type`).
- Headers under `index/` depend on `utils/io.hh` for binary serialization via `operator<<`/`operator>>` friends.
