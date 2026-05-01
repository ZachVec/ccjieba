#pragma once

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace ccjieba {

class PreSegment {
  std::u32string symbols_;
  std::u32string_view sentence_;
  friend class PreSegmenter;
  PreSegment(std::u32string_view symbols, std::u32string_view sentence) : symbols_(symbols), sentence_(sentence) {}

 public:
  class const_iterator {  // NOLINT(readability-identifier-naming)
    size_t u_;
    size_t v_;
    std::u32string_view symbols_;
    std::u32string_view sentence_;
    friend class PreSegment;

    const_iterator(size_t u, std::u32string_view symbols, std::u32string_view sentence);

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::u32string_view;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    auto operator*() const -> value_type;

    auto operator==(const const_iterator &other) const -> bool;

    auto operator!=(const const_iterator &other) const -> bool;

    auto operator++() -> const_iterator &;

    auto operator++(int) -> const_iterator;

   private:
    const_iterator(std::u32string_view symbols, std::u32string_view sentence);
  };

  auto begin() const -> const_iterator;

  auto end() const -> const_iterator;
};

class PreSegmenter {
  std::u32string symbols_;

 public:
  PreSegmenter(std::u32string symbols);
  auto operator()(std::u32string_view sentence) const -> PreSegment;
  auto reset(std::u32string symbols) -> void;
};

}  // namespace ccjieba
