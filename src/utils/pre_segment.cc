#include "utils/pre_segment.hh"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace ccjieba {

PreSegment::const_iterator::const_iterator(size_t u, std::u32string_view symbols, std::u32string_view sentence)
  : u_(u), v_(sentence.find_first_of(symbols, u)), symbols_(symbols), sentence_(sentence) {
  if (u_ == v_) {
    ++v_;
  }
}

auto PreSegment::const_iterator::operator*() const -> value_type {
  return sentence_.substr(u_, v_ - u_);
}

auto PreSegment::const_iterator::operator==(const const_iterator &other) const -> bool {
  return u_ == other.u_ and symbols_ == other.symbols_ and sentence_ == other.sentence_;
}

auto PreSegment::const_iterator::operator!=(const const_iterator &other) const -> bool {
  return !(*this == other);
}

auto PreSegment::const_iterator::operator++() -> const_iterator & {
  if (u_ >= sentence_.size()) {
    return *this;
  }
  if (v_ == std::u32string_view::npos) {
    u_ = sentence_.size();
    return *this;
  }
  u_ = v_;
  v_ = sentence_.find_first_of(symbols_, u_);
  if (v_ == u_) {
    ++v_;
  }
  return *this;
}

auto PreSegment::const_iterator::operator++(int) -> const_iterator {
  const_iterator it = *this;
  ++(*this);
  return it;
}

auto PreSegment::begin() const -> const_iterator {
  return const_iterator(0, symbols_, sentence_);
}

auto PreSegment::end() const -> const_iterator {
  return const_iterator(sentence_.size(), symbols_, sentence_);
}

PreSegmenter::PreSegmenter(std::u32string symbols) : symbols_(std::move(symbols)) {}

auto PreSegmenter::operator()(std::u32string_view sentence) const -> PreSegment {
  return PreSegment(symbols_, sentence);
}

auto PreSegmenter::reset(std::u32string symbols) -> void {
  symbols_ = std::move(symbols);
}

}  // namespace ccjieba
