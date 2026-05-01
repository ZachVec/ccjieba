#include "index/stop_words.hh"

#include <istream>
#include <string>
#include <string_view>
#include <utility>

#include "utils/io.hh"

namespace ccjieba {

auto StopWordsDictionary::operator[](std::string_view str) const -> bool {
  decltype(stop_words_)::key_equal key_eq = stop_words_.key_eq();
  auto bucket_id = stop_words_.hash_function()(str) % stop_words_.bucket_count();
  for (auto it = stop_words_.begin(bucket_id); it != stop_words_.end(bucket_id); ++it) {
    if (key_eq(*it, str)) {
      return true;
    }
  }
  return false;
}

auto operator>>(std::istream &is, StopWordsDictionary &stop_words) -> std::istream & {
  std::string line;
  while (std::getline(is, line)) {
    stop_words.stop_words_.emplace(std::move(line));
  }
  return is;
}

auto operator>>(bistream &is, StopWordsDictionary &stop_words) -> bistream & {
  std::string term;
  while (is >> term) {
    stop_words.stop_words_.emplace(std::move(term));
  }
  return is;
}

auto operator<<(bostream &os, const StopWordsDictionary &stop_words) -> bostream & {
  for (const auto &term : stop_words.stop_words_) {
    os << term;
  }
  return os;
}

}  // namespace ccjieba
