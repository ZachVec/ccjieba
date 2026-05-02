# HMMSegment

Pure HMM-based segmentation using Viterbi decoding.

**Strategy:** Models Chinese text as a Hidden Markov Model with 4 states:
- **B** — Begin of word
- **E** — End of word
- **M** — Middle of word
- **S** — Single-character word

Uses the HMM model file (`hmm_model.utf8`) for initial state probabilities, transition probabilities, and emission probabilities.
