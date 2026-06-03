#pragma once

#include <algorithm>
#include <memory>

template <typename T> struct runtime_array {
  using value_type = T;

private:
  std::unique_ptr<T[]> array;
  std::size_t _size;

public:
  runtime_array() : _size{0} {}
  runtime_array(const std::size_t size)
      : array{std::make_unique<T[]>(size)}, _size{size} {}
  runtime_array(std::initializer_list<T> list)
      : array{std::make_unique<T[]>(list.size())}, _size{list.size()} {
    std::copy(list.begin(), list.begin() + _size, array.get());
  }
  // runtime_array(std::unique_ptr<T[]> &&poly, const std::size_t size)
  //     : array{std::move(poly)}, _size{size} {}
  template <typename Iter>
  runtime_array(Iter first, Iter last)
      : array{std::make_unique<T[]>(std::distance(first, last))},
        _size{static_cast<std::size_t>(std::distance(first, last))} {
    std::copy(first, last, array.get());
  }

  runtime_array(const runtime_array &that)
      : array{std::make_unique<T[]>(that._size)}, _size{that._size} {
    std::copy(that.array.get(), that.array.get() + _size, array.get());
  }
  runtime_array &operator=(const runtime_array &that) {
    array = std::make_unique<T[]>(that._size);
    _size = that._size;
    std::copy(that.array.get(), that.array.get() + _size, array.get());
    return *this;
  }
  runtime_array(runtime_array &&) = default;
  runtime_array &operator=(runtime_array &&) = default;

  T &operator[](const std::size_t i) { return *(array.get() + i); }
  const T &operator[](const std::size_t i) const { return *(array.get() + i); }

  T *begin() { return array.get(); }
  const T *begin() const { return array.get(); }
  const T *cbegin() const { return array.get(); }
  T *end() { return begin() + _size; }
  const T *end() const { return begin() + _size; }
  const T *cend() const { return cbegin() + _size; }

  T *data() { return array.get(); }
  const T *data() const { return array.get(); }

  std::size_t size() const { return _size; }
};