# POS Tagging

## `Jieba::tag_sentence`

```cpp
auto tag_sentence(std::string_view str)
    -> std::vector<std::pair<std::string, std::string_view>>;
```

Segment a sentence and tag each word with its part-of-speech. Dictionary-matched words use the tag from the dictionary; unmatched words are auto-classified.

```cpp
for (auto &[word, tag] : jieba.tag_sentence("我是蓝翔技工"))
    std::cout << word << "/" << tag << " ";
// 我/r 是/v 蓝翔/nz 技工/n
```

## `Jieba::tag_word`

```cpp
auto tag_word(std::string_view str) -> std::string_view;
```

Return the POS tag for a single word. Falls back to auto-classification when the word is not in the dictionary:

```cpp
jieba.tag_word("手扶拖拉机");  // "n"    ← dictionary match
jieba.tag_word("CEO");        // "eng"  ← English
jieba.tag_word("123");        // "m"    ← number
jieba.tag_word("龘龘");       // "x"    ← unknown
```
