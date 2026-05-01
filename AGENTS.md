# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Test

```bash
cmake -B build && cmake --build build    # build everything (lib + demo + tests)
cmake --build build --target unittest    # build only tests
cd build && ctest --output-on-failure    # run all tests
cd build && ./tests/unittest --gtest_filter='JiebaTest.*'  # run a single test suite
cd build && ./bin/demo                   # run the demo
```

For release builds: `cmake -B build -DCMAKE_BUILD_TYPE=Release`.
Shared library: `cmake -B build -DBUILD_SHARED_LIBS=ON`.

## Architecture

C++17, zero external dependencies (GoogleTest is fetched at build time via `FetchContent`). The library is a static/header-only hybrid — algorithms are templates in headers, data structures are compiled in `.cc` files.

### Segmentation pipeline

```
raw UTF-8 sentence
  → utf8_to_utf32 (all internal processing on char32_t)
  → PreSegmenter (splits on punctuation: U" \t，。")
  → [MPSegment | HMMSegment | MixSegment | FullSegment | QuerySegment] (operates on each piece)
  → utf32_to_utf8 back to std::string
```

### Five segmentation algorithms (src/algo/)

- **MPSegment** (`mp.hh`) — Max-probability (dynamic programming on Trie DAG). Dictionary-only.
- **HMMSegment** (`hmm.hh`) — HMM Viterbi decoding. HMM-only, no dictionary lookup.
- **MixSegment** (`mix.hh`) — Dictionary match + HMM fallback on OOV characters. The default algorithm.
- **FullSegment** (`full.hh`) — Enumerate ALL dictionary words in the sentence.
- **QuerySegment** (`query.hh`) — Like FullSegment but also enumerates substrings of configurable lengths for query recall.

### Key data structures (src/include/index/)

- **Trie** (`trie.hh`) — Prefix tree storing dictionary words with weights and POS tags. Supports `match` (exact lookup) and `search` (DAG of all possible word boundaries). Has a separate `UserTrie` for user-defined additions, tracked via `user_char_` set.
- **HMModel** (`hmmodel.hh`) — HMM parameters (start probs, transition probs, emission probs) over 4 states: B, E, M, S. Exposes a `Context` callable for Viterbi.
- **InverseDocumentFrequence** (`idf.hh`) — IDF scores for keyword extraction.
- **StopWordsDictionary** (`stop_words.hh`) — Stop word set for keyword extraction.

### Viterbi is decoupled from HMM

`Viterbi<ModelT>` (`src/algo/viterbi.hh`) is a generic template that works with any model satisfying a compile-time interface (must expose `LOWEST`, `NSTATE`, `DSTATE` constants and a `Context` with `pstart`, `ptrans`, `pemits`, `is_end` methods). This enables swapping in CRF or other sequence models later.

### Jieba facade

`Jieba` (`src/include/jieba.hh`) owns the four data indices (`Trie`, `HMModel`, IDF, stop words) and provides:
- `Jieba::cut<Algo>()` — template method dispatching to one of the five segmenters
- `Jieba::extract()` — keyword extraction via `KeywordExtractor`
- `Jieba::tag_sentence()` — POS tagging via `Tagger`
- `Jieba::tag_word()` — single-word tag lookup

Each algorithm takes its dependencies by reference (the indices live on `Jieba`). Algorithms are stateless functors: construct with references, call `operator()(u32string_view) -> vector<u32string_view>`.

### Binary serialization (src/include/utils/io.hh)

Custom `bostream`/`bistream` provide portable binary I/O alongside standard `istream >>` / `ostream <<` text operators. All index types support both formats (text for human-readable data, binary for faster loading). If only binary I/O is needed for a type, only the `bistream/bostream` operators need to exist.

### Code style

Based on Google style with right-aligned pointers, 120-column limit, C++17. See `.clang-format`.
