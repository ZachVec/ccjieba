#include "utils/io.hh"

#include <cstring>

namespace ccjieba {

auto operator<<(bostream &os, bool value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, short value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, int value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, long value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, long long value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, unsigned short value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, unsigned int value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, unsigned long value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, unsigned long long value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, float value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, double value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, long double value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, char value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, signed char value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, unsigned char value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, char32_t value) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(&value), sizeof(value));
}

auto operator<<(bostream &os, const char *value) -> bostream & {
  size_t size = strlen(value);
  os << size;
  return os.dump(reinterpret_cast<const std::byte *>(value), size);
}

auto operator>>(bistream &is, bool &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, short &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, int &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, long &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, long long &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, unsigned short &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, unsigned int &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, unsigned long &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, unsigned long long &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, float &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, double &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, long double &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, char &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, signed char &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, unsigned char &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

auto operator>>(bistream &is, char32_t &value) -> bistream & {
  using value_t = std::decay_t<decltype(value)>;
  if (is.size() >= sizeof(value)) {
    value = *reinterpret_cast<const value_t *>(is.data());
  }
  return is.step(sizeof(value));
}

}  // namespace ccjieba
