# Getting Started

## Build

```bash
cmake -B build && cmake --build build        # static lib + demo
cmake -B build -DBUILD_TESTING=ON            # static lib + demo + tests
cd build && ctest                            # run tests
```

## Basic Usage

Include `jieba.hh`, construct a `Jieba` object, load data via `istream`:

```cpp
#include <fstream>
#include <jieba.hh>

ccjieba::Jieba jieba;
std::ifstream("data/jieba.dict.utf8") >> jieba.trie_;
std::ifstream("data/hmm_model.utf8") >> jieba.hmm_;
std::ifstream("data/idf.utf8") >> jieba.idf_;
std::ifstream("data/stop_words.utf8") >> jieba.stop_words_;
```

Data files are located in `data/` (configured at compile time via the `DATA_ROOT` macro):

```
data/
├── jieba.dict.utf8    # dictionary (required)
├── hmm_model.utf8     # HMM model (required)
├── idf.utf8           # IDF weights (required for keywords)
└── stop_words.utf8    # stop words (required for keywords)
```
