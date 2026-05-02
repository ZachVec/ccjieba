/// @file pre_segment.hh
/// @brief Pre-segmentation: splits UTF-32 text on delimiter characters before segmentation.

#pragma once

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace ccjieba {

/// @brief A view over a sentence split by delimiter characters.
///
/// Created by PreSegmenter::operator() and iterated via const_iterator.
/// Each dereference yields the next non-delimiter substring.
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

    /// @brief The current non-delimiter substring.
    auto operator*() const -> value_type;

    auto operator==(const const_iterator &other) const -> bool;

    auto operator!=(const const_iterator &other) const -> bool;

    /// @brief Advance to the next non-delimiter substring.
    auto operator++() -> const_iterator &;

    auto operator++(int) -> const_iterator;

   private:
    const_iterator(std::u32string_view symbols, std::u32string_view sentence);
  };

  auto begin() const -> const_iterator;

  auto end() const -> const_iterator;
};

/// @brief Functor that splits sentences on a configured set of delimiter characters.
///
/// The split is performed lazily via the returned PreSegment iterator.
/// Default delimiters in practice are space, tab, comma, and Chinese full stop (。).
class PreSegmenter {
  std::u32string symbols_;

 public:
  /// @param symbols The set of delimiter characters (UTF-32).
  PreSegmenter(std::u32string symbols);

  /// @brief Create an iterable PreSegment view over @p sentence.
  auto operator()(std::u32string_view sentence) const -> PreSegment;

  /// @brief Replace the delimiter set.
  auto reset(std::u32string symbols) -> void;
};

}  // namespace ccjieba
