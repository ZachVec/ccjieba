## src/algo -- Segmentation and Tagging Algorithms

This directory contains the algorithmic core of ccjieba: five segmenters (dictionary-based, HMM-based, and hybrid), a POS tagger, a keyword extractor, and the Viterbi template. Each segmenter is a functor holding const references to shared index objects, constructed with dependencies then called via `operator()(u32string_view) -> vector<u32string_view>`.

### Files

- **viterbi.hh** — Generic Viterbi algorithm template parameterized on a model type providing `pstart`, `ptrans`, `pemits`, and `is_end`.
- **internal.hh** — Shared types (`Segment`, `Segments`, `Graph`, `Edge`) and declarations for `mp()` (DAG max-probability) and `mix()` (hybrid DAG+HMM).
- **mp.hh / mp.cc** — `MPSegment`: dictionary-only max-probability segmentation via reverse DP over the trie DAG.
- **hmm.hh / hmm.cc** — `HMMSegment`: segments unregistered character runs using the HMM model, delegating to `Viterbi`.
- **mix.hh / mix.cc** — `MixSegment`: hybrid segmenter that runs `mp()` then `mix()` to pass unknown-word spans through HMM.
- **full.hh / full.cc** — `FullSegment`: returns ALL possible dictionary words found in the trie (not just the optimal path).
- **query.hh / query.cc** — `QuerySegment`: extends MixSegment output with finer-grained sub-segments at configurable lengths (default 2-3 chars).
- **tagger.hh / tagger.cc** — `Tagger`: segments text and assigns POS tags from the trie, falling back to heuristic classification.
- **extractor.hh / extractor.cc** — `KeywordExtractor`: ranks tokenized segments by TF-IDF weight and returns top N keywords with offsets.

### Key patterns

- `mp()` and `mix()` live as free functions in `ccjieba` namespace, shared across segmenters.
- `QuerySegment` and `Tagger` layer on top of `MixSegment` (`mp()` + `mix()`) rather than reimplementing the pipeline.
- All segmenters operate on `std::u32string_view` to avoid copying; returned views alias the input buffer.
