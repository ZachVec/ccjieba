# MixSegment

The default segmentation algorithm. Combines dictionary-based max-probability segmentation with HMM for out-of-vocabulary (OOV) word handling.

**Strategy:** First applies `MPSegment` using the dictionary trie, then handles any remaining unsegmented characters with `HMMSegment` (Viterbi decoding).

This is the recommended algorithm for general-purpose segmentation.
