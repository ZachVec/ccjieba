# CLAUDE.md

## Build & Test

```bash
cmake -B build && cmake --build build    # build everything (lib + demo + tests)
cd build && ctest --output-on-failure    # run all tests
cd build && ./tests/unittest --gtest_filter='JiebaTest.*'  # single suite
cd build && ./bin/demo                   # run demo
```

Release: `-DCMAKE_BUILD_TYPE=Release`. Shared lib: `-DBUILD_SHARED_LIBS=ON`.

## Architecture

C++17, zero external dependencies (GoogleTest via `FetchContent`). Static/header-only hybrid — algorithms are templates in headers, data structures in `.cc`.

### Pipeline

```
UTF-8 → utf8_to_utf32 → PreSegmenter → [segmenter] → utf32_to_utf8
```

Five segmenters: MPSegment (DP on Trie DAG), HMMSegment (Viterbi), MixSegment (default, dictionary+HMM fallback), FullSegment (all dictionary words), QuerySegment (FullSegment + substrings). `Jieba` facade owns four indices (Trie, HMModel, IDF, stop words) and dispatches via `cut<Algo>()`, `extract()`, `tag_sentence()`, `tag_word()`.

### Directory map

| Directory | Contents | Details |
|-----------|----------|---------|
| `src/algo/` | Segmenters, tagger, extractor, Viterbi | `src/algo/CLAUDE.md` |
| `src/include/` | Public API headers (`Jieba`, `Keyword`) | `src/include/CLAUDE.md` |
| `src/include/index/` | Index headers (Trie, HMModel, IDF, stop words) | `src/include/index/CLAUDE.md` |
| `src/include/utils/` | Binary I/O (`bostream`/`bistream`) | `src/include/utils/CLAUDE.md` |
| `src/index/` | Index implementations (text + binary I/O) | `src/index/CLAUDE.md` |
| `src/utils/` | DAG, pre-segmentation, UTF codec, I/O operators | `src/utils/CLAUDE.md` |

### Code style

Google-based, right-aligned pointers, 120-column limit, C++17. Trailing-return-type syntax. See `.clang-format`.
