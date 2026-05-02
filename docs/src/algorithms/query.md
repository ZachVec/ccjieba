# QuerySegment

Extended full segmentation optimized for search queries.

**Strategy:** Applies `FullSegment` to find all dictionary matches, then additionally generates substrings of configurable length to maximize search recall.

Controlled by `max_word_length` parameter (default 500) passed to `Jieba::cut`.
