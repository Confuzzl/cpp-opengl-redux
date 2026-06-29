#pragma once

#include <fmt/base.h>
#include <fmt/color.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <concepts>
#include <limits>
#include <random>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

#if defined(_MSC_VER) && !defined(__clang__) // MSVC
#define UNREACHABLE __assume(false);
#else // GCC, Clang
#define UNREACHABLE __builtin_unreachable();
#endif

#define MOVE_ONLY_TYPE(type)                                                   \
  type(const type &) = delete;                                                 \
  type(type &&o) noexcept;                                                     \
  type &operator=(const type &) = delete;                                      \
  type &operator=(type &&o) noexcept;
#define DEFAULT_MOVE_ONLY_TYPE(type)                                           \
  type(const type &) = delete;                                                 \
  type(type &&o) noexcept = default;                                           \
  type &operator=(const type &) = delete;                                      \
  type &operator=(type &&o) noexcept = default;

// C++20 ONLY
template <std::size_t N> struct nttp_str {
  char m[N];
  constexpr nttp_str(const char (&s)[N]) { std::copy_n(s, N, m); }
};

// https://stackoverflow.com/a/42915152/31514738
template <class T, std::size_t N>
auto make_vector(std::array<T, N> &&a) -> std::vector<T> {
  return {std::make_move_iterator(std::begin(a)),
          std::make_move_iterator(std::end(a))};
}
template <class... T> auto make_vector(T &&...t) {
  return make_vector(std::to_array({std::forward<T>(t)...}));
}

template <fmt::color C, nttp_str PREFIX, typename... Args>
void print_debug(const fmt::format_string<Args...> str, Args &&...args) {
  fmt::print(fmt::fg(C), PREFIX.m);
  fmt::println(str, std::forward<Args>(args)...);
}
template <typename... Args>
void print_info(const fmt::format_string<Args...> str, Args &&...args) {
  print_debug<fmt::color::sky_blue, "INFO: ">(str, std::forward<Args>(args)...);
}
template <typename... Args>
void print_warn(const fmt::format_string<Args...> str, Args &&...args) {
  print_debug<fmt::color::yellow, "WARNING: ">(str,
                                               std::forward<Args>(args)...);
}
template <typename... Args>
void print_err(const fmt::format_string<Args...> str, Args &&...args) {
  print_debug<fmt::color::red, "ERROR: ">(str, std::forward<Args>(args)...);
}
template <typename T = float> T random_float(const T a, const T b) {
  static std::random_device rd;
  static std::mt19937 mt{rd()};

  return std::uniform_real_distribution<T>{a, b}(mt);
}
template <typename T = int>
T random_int(const T a = std::numeric_limits<T>::min(),
             const T b = std::numeric_limits<T>::max()) {
  static_assert(std::is_integral_v<T>, "type must be integral");
  static std::random_device rd;
  static std::mt19937 mt{rd()};

  return std::uniform_int_distribution<T>{a, b}(mt);
}
glm::vec2 random_vec2();

glm::mat3 normal_matrix(const glm::mat3 &mat);

constexpr auto pow(const int a, const int n) {
  int out = 1;
  for (int i = 0; i < n; i++) {
    out *= a;
  }
  return out;
}
