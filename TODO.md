# TODO

## 健壮性

- **编码错误静默吞没** — `utf8_to_utf32` 返回 `false` 时，`Jieba::cut` 和 `tag_sentence` 直接返回空结果，调用方无法区分「空输入」和「编码损坏」。
- **词典加载的半成功状态** — `trie.cc` 文本解析遇到格式错误时设 `badbit`，但同一行之前的数据可能已写入——错误恢复后 trie 处于部分写入状态。
- **IDF 重复键检测不完整** — `idf.cc:42` 遇到重复词条只设 `badbit` 不返回，后续数据仍继续解析写入。

## 效率（非紧急，可选优化）

- **Trie 节点内存布局** — `unique_ptr<unordered_map<char32_t, Node>>` 每个节点一次堆分配。大词典下碎片严重、缓存局部性差。可考虑 flat array 或 arena 分配。
- **二进制序列化不可跨平台移植** — `io.hh` 中的 `bostream`/`bistream` 直接 dump/read 多字节类型的原始字节，不处理字节序。当前仅测试中使用，如未来需要跨架构共享数据需修复。

## 架构

- **Trie 职责混杂** — `Info` 结构同时承载权重（分词用）和 POS 标签（标注用），不使用标签的场景仍有内存开销。

## 功能完整性（对标 cppjieba）

- **`cut()` 缺少 Word 偏移量输出** — cppjieba 五种 Cut 方法都有 `vector<Word>` 重载（含 `{word, begin, end}` 字节偏移）。ccjieba 只返回 `vector<string>`，调用方无法获取词在原文中的位置。
- **缺少运行时 `InsertUserWord` / `DeleteUserWord`** — cppjieba 的 `Jieba` 可直接调 `InsertUserWord("新词", freq, tag)` 和 `DeleteUserWord("词")`。ccjieba 只能通过 `UserTrie >> istream` 从流批量加载，无单条增删 API。
- **缺少 `TextRankExtractor`** — cppjieba 有基于 PageRank 的关键词提取算法，支持可配置的窗口 span 和迭代次数。ccjieba 仅有 TF-IDF 提取。
- **缺少 `ResetSeparators`** — cppjieba 允许运行时修改分隔符。ccjieba 硬编码 `U" \t，。"` 在 `jieba.cc` 中，无法动态调整。
- **`cut()` 缺少 `hmm` 开关** — cppjieba 的 `Cut(s, words, hmm=false)` 可一键关闭 HMM。ccjieba 需用模板参数 `cut<MPSegment>()` 替代，不够直观。
- **缺少 `Find()` 快捷查询** — cppjieba 的 `Jieba::Find(word) -> bool`。ccjieba 需通过 `trie_.match()` 间接访问。
- **缺少 `UserWordWeightOption`** — cppjieba 支持 `WordWeightMin` / `WordWeightMedian` / `WordWeightMax` 控制用户词默认权重。ccjieba 默认取 trie 最小权重，不可配置。
