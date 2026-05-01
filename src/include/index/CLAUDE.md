## src/include/index — Data index headers

Core data structures that all segmentation algorithms depend on. Each is deserialized from an index file via `operator>>` and exposes a minimal query interface.

### Files

- **trie.hh** — `Trie`: Unicode-trie (keyed by `char32_t`) mapping phrases to `Info{weight, tag}`. Supports prefix `match()`, DAG-building `search()`, and DFS `const_iterator`. Also exposes a `UserTrie` friend class for user-dict insertion.
- **hmmodel.hh** — `HMModel`: 4-state (B/E/M/S) HMM for Viterbi. Stores start probs, 4×4 transition matrix, and per-state emission maps. Provides a `Context` callable bound to an input string; `is_end()` exploits the encoding (odd = end state).
- **idf.hh** — `InverseDocumentFrequence`: Maps UTF-8 tokens to IDF scores. Returns `average_` as fallback for unknown tokens.
- **stop_words.hh** — `StopWordsDictionary`: `unordered_set` of stop-word tokens, queried via `operator[]`.

### Conventions

- All headers depend on `utils/io.hh` for binary `bostream`/`bistream` serialization.
- Heterogeneous lookup via transparent hashers (`std::hash<string_view>`, `std::equal_to<>`).
