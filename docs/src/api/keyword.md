# Keyword Extraction

## `Jieba::extract`

```cpp
auto extract(std::string_view str, size_t topN) -> std::vector<Keyword>;
```

Extract the top-N keywords ranked by TF-IDF weight (descending).

```cpp
auto keywords = jieba.extract(
    "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。",
    5
);
for (auto &kw : keywords)
    std::cout << kw.word << ": " << kw.weight << "\n";
// CEO: 11.7392
// 升职: 10.8562
// 加薪: 10.6426
// 手扶拖拉机: 10.0089
// 巅峰: 9.49396
```

## `Keyword` struct

```cpp
struct Keyword {
    double weight;                  // TF-IDF weight
    std::string word;               // keyword text
    std::vector<std::size_t> offsets; // byte offsets in original string
};
```

Requires `idf.utf8` and `stop_words.utf8` to be loaded.
