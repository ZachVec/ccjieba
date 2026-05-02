# MPSegment

Pure dictionary-based max-probability segmentation.

**Strategy:** Builds a DAG of all possible dictionary matches, then finds the maximum probability path using dynamic programming. Word probabilities are derived from dictionary frequencies.

Does not handle OOV words — characters not found in the dictionary are left unsegmented.
