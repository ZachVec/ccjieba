# ccjieba

C++ 中文分词库，受 [cppjieba](https://github.com/yanyiwu/cppjieba) 启发的重构版本。C++17，零外部依赖。

## 构建

```bash
cmake -B build && cmake --build build        # 静态库 + demo
cmake -B build -DBUILD_TESTING=ON            # 静态库 + demo + 单元测试
cd build && ctest                            # 运行测试
```

## 使用

引入 `jieba.hh`，构造 `Jieba` 对象，通过 `istream` 加载数据即可使用。

数据文件放在 `data/` 目录（编译时通过 `DATA_ROOT` 宏指定）：

```
data/
├── jieba.dict.utf8    # 词典（必需）
├── hmm_model.utf8     # HMM 模型（必需）
├── idf.utf8           # IDF 词频（关键词提取必需）
└── stop_words.utf8    # 停用词（关键词提取必需）
```

```cpp
#include <fstream>
#include <jieba.hh>

ccjieba::Jieba jieba;
std::ifstream("data/jieba.dict.utf8") >> jieba.trie_;
std::ifstream("data/hmm_model.utf8") >> jieba.hmm_;
std::ifstream("data/idf.utf8") >> jieba.idf_;
std::ifstream("data/stop_words.utf8") >> jieba.stop_words_;
```

### `Jieba::cut`

```cpp
auto Jieba::cut<Algo>(std::string_view str, std::optional<size_t> max_word_length = 500)
    -> std::vector<std::string>;
```

分词。模板参数 `Algo` 为五种算法之一，默认 `MixSegment`。

| 算法 | 说明 |
|------|------|
| `MixSegment`（默认） | 词典最大概率 + HMM 处理未登录字 |
| `MPSegment` | 纯词典最大概率 |
| `HMMSegment` | 纯 HMM Viterbi 解码 |
| `FullSegment` | 枚举全量词典匹配 |
| `QuerySegment` | FullSegment + 指定长度子串，用于搜索召回 |

```cpp
auto words = jieba.cut("我来到北京清华大学");
// → {"我", "来到", "北京", "清华大学"}

jieba.cut<ccjieba::FullSegment>("我来到北京清华大学");
// → {"我", "来到", "北京", "清华", "清华大学", "华大", "大学"}
```

### `Jieba::extract`

```cpp
auto Jieba::extract(std::string_view str, size_t topN) -> std::vector<Keyword>;
```

提取 topN 个关键词，按 TF-IDF 权重降序排列。

```cpp
auto keywords = jieba.extract("我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。", 5);
for (auto &kw : keywords)
    std::cout << kw.word << ": " << kw.weight << "\n";
// CEO: 11.7392
// 升职: 10.8562
// 加薪: 10.6426
// 手扶拖拉机: 10.0089
// 巅峰: 9.49396
```

`Keyword` 结构：

```cpp
struct Keyword {
    double weight;                // TF-IDF 权重
    std::string word;             // 关键词文本
    std::vector<std::size_t> offsets;  // 在原文中的字节偏移
};
```

### `Jieba::tag_sentence`

```cpp
auto Jieba::tag_sentence(std::string_view str)
    -> std::vector<std::pair<std::string, std::string_view>>;
```

对句子分词并标注词性。词典命中的词使用词典中的 tag，未命中的自动分类为 `eng` / `m` / `x`。

```cpp
for (auto &[word, tag] : jieba.tag_sentence("我是蓝翔技工"))
    std::cout << word << "/" << tag << " ";
// 我/r 是/v 蓝翔/nz 技工/n
```

### `Jieba::tag_word`

```cpp
auto Jieba::tag_word(std::string_view str) -> std::string_view;
```

返回单个词的词性标签，不命中词典时自动分类：

```cpp
jieba.tag_word("手扶拖拉机");  // "n"    ← 词典命中
jieba.tag_word("CEO");        // "eng"  ← 英文
jieba.tag_word("123");        // "m"    ← 数字
jieba.tag_word("龘龘");       // "x"    ← 未知
```

### 用户词典

通过 `Trie::user()` 获取 `UserTrie`，用 `istream >>` 加载，增量添加词条而不重建词典：

```cpp
std::ifstream("user.dict.utf8") >> jieba.trie_.user();
```

用户词典文件格式，频率和词性均可选：

```
云计算 5 n
云计算 n
云计算
```

## 提交前检查

项目配置了 pre-commit hook，提交前自动运行 clang-format 和 clang-tidy：

```bash
# 克隆后启用
ln -sf ../../scripts/pre-commit .git/hooks/pre-commit
```

- **clang-format** — 自动格式化暂存的文件并重新暂存
- **clang-tidy** — 检查暂存的 `.cc` 文件，有警告时阻止提交

需要 `compile_commands.json` 时 hook 会自动生成，也可手动提前生成以加速首次提交：

```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

## 数据文件格式

**词典** (`jieba.dict.utf8`) — `词语 词频 词性`，每行一条：

```
清华大学 5 ns
来到 3 v
```

**HMM 模型** (`hmm_model.utf8`) — 概率取 log 后的值。首行为 4 个初始状态概率 (B E M S)，接着 4×4 转移概率矩阵，然后 4 行发射概率（格式 `字:概率,字:概率,...`）：

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

**IDF** (`idf.utf8`) — `词语 IDF值`，每行一条：

```
来到 10.5
北京 8.2
```

**停用词** (`stop_words.utf8`) — 每行一个词：

```
的
了
在
```
