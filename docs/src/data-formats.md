# Data Formats

## Dictionary (`jieba.dict.utf8`)

Format: `word frequency tag`, one entry per line.

```
清华大学 5 ns
来到 3 v
```

- `frequency`: integer, used by MPSegment for probability calculation
- `tag`: optional part-of-speech tag

## HMM Model (`hmm_model.utf8`)

Log-probability values. Structure:

1. **Line 1**: 4 initial state probabilities (B, E, M, S)
2. **Lines 2-5**: 4×4 transition probability matrix
3. **Lines 6-9**: Emission probabilities for each state, format `char:prob,char:prob,...`

```
-0.2626 -3.14e+100 -3.14e+100 -1.4653
-3.14e+100 -0.5108 -0.9163 -3.14e+100
-0.5897 -3.14e+100 -3.14e+100 -0.8085
-3.14e+100 -0.3334 -1.2604 -3.14e+100
-0.7212 -3.14e+100 -3.14e+100 -0.6659
耀:-10.46,蘄:-11.02
耀:-9.27,蘄:-17.33
耀:-8.48,蘄:-14.37
耀:-11.22,蘄:-10.01
```

`-3.14e+100` represents negative infinity (impossible transitions).

## IDF (`idf.utf8`)

Format: `word IDF_value`, one entry per line.

```
来到 10.5
北京 8.2
```

Used for TF-IDF keyword extraction.

## Stop Words (`stop_words.utf8`)

One word per line:

```
的
了
在
```

Filtered out during keyword extraction.
