## src/index — Index implementation files

Compiled implementations of the data structures declared in `src/include/index/`. Each `.cc` file implements text (`operator>>(istream&)`) and binary (`operator>>(bistream&)`, `operator<<(bostream&)`) I/O plus query methods.

### Files

- **hmmodel.cc** — HMModel: text format uses space-delimited floats and comma-separated `utf8char:freq` pairs; binary writes fixed-size arrays then unordered_map entries.
- **idf.cc** — InverseDocumentFrequence: parses `word freq` lines; computes `average_` fallback. `operator[]` uses manual bucket walk for heterogeneous `string_view` lookup.
- **stop_words.cc** — StopWordsDictionary: one word per line; binary reads until EOF (no size prefix). `operator[]` uses bucket-probe membership test.
- **trie.cc** — Trie: text format `phrase freq tag` (3 columns); frequencies are log-transformed relative to `total_frequence_`. Binary writes precomputed log-scaled entries. Also implements `UserTrie` loading (1-3 column variants), `upsert`/`match`/`search`, and DFS `const_iterator`.

### Key design detail

All four types support two I/O channels: `istream` for human-readable text and `bistream`/`bostream` for compact binary. The text formats are consumed during initialization; binary formats are used for faster reloading.
