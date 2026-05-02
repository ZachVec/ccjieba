# Jieba & Cut

The `Jieba` class is the main entry point. It holds the dictionary trie, HMM model, IDF table, and stop word set.

## `Jieba::cut`

```cpp
template <AlgoConcept Algo = MixSegment>
auto cut(std::string_view str, std::optional<size_t> max_word_length = 500)
    -> std::vector<std::string>;
```

Segment `str` into words. The template parameter `Algo` selects the segmentation algorithm (default: `MixSegment`).

`max_word_length` limits the maximum word length considered during dictionary matching.

```cpp
auto words = jieba.cut("我来到北京清华大学");
// → {"我", "来到", "北京", "清华大学"}

jieba.cut<ccjieba::FullSegment>("我来到北京清华大学");
// → {"我", "来到", "北京", "清华", "清华大学", "华大", "大学"}
```

| Algorithm | Description |
|-----------|-------------|
| `MixSegment` (default) | Dictionary MPS + HMM for OOV words |
| `MPSegment` | Pure dictionary max-probability |
| `HMMSegment` | Pure HMM Viterbi decoding |
| `FullSegment` | Enumerate all dictionary matches |
| `QuerySegment` | FullSegment + substrings of given length, for search recall |
