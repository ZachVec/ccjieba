## src/utils — Utility library

Shared utilities: DAG graph structure, UTF-8/UTF-32 codec, pre-segmentation, string helpers, and binary I/O operators.

### Files

- **dag.hh** — `DiGraph<T>`: templated adjacency-list directed graph (header-only).
- **pre_segment.hh / pre_segment.cc** — `PreSegmenter` splits UTF-32 sentences on delimiter characters via `PreSegment::const_iterator`.
- **str_utils.hh / str_utils.cc** — `split()` and `strip()` templates over `basic_string_view`; manual UTF-8/32 codec (no libiconv).
- **io.cc** — Out-of-line `operator<<`/`operator>>` bodies for all primitive types on `bostream`/`bistream` (header declared in `src/include/utils/io.hh`).

### Conventions

- All code in `namespace ccjieba`. Headers use `#pragma once`. Trailing-return-type syntax.
- Member variables: `snake_case_` (trailing underscore). Iterators are nested `const_iterator` classes.
- `PreSegment::const_iterator` is an `input_iterator_tag`.
