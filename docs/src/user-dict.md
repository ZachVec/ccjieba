# User Dictionary

Add custom words without rebuilding the main dictionary.

## Loading

```cpp
std::ifstream("user.dict.utf8") >> jieba.trie_.user();
```

## File Format

One entry per line. Frequency and tag are optional:

```
云计算 5 n
云计算 n
云计算
```

## Effect

Words added via the user dictionary take priority over dictionary entries during segmentation. This is useful for domain-specific terms, new words, or correcting segmentation errors.
