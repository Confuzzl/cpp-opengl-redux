#pragma once

#include "glad/gl.h"
#include <fmt/base.h>
#include <fmt/color.h>
#include <glm/glm.hpp>

#include <algorithm>
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

// C++20 ONLY
template <std::size_t N> struct nttp_str {
  char m[N];
  constexpr nttp_str(const char (&s)[N]) { std::copy_n(s, N, m); }
};

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
// void print_warn(const fmt::format_string<> str);
// void print_err(const fmt::format_string<> str);

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

namespace GL {
std::string_view getErrorName();

constexpr glm::u16vec2 uv_as_short(const glm::vec2 uv) {
  constexpr GLushort MAX = -1;
  return glm::u16vec2{uv.x * MAX, uv.y * MAX};
}

namespace detail {
struct gen_2_10_10_10_rev {
  unsigned int storage = 0;

  constexpr gen_2_10_10_10_rev() = default;
  constexpr gen_2_10_10_10_rev(const unsigned int x, const unsigned int y,
                               const unsigned int z, const unsigned int w = 0)
      : storage{(x & 0b11'1111'1111) | ((y & 0b11'1111'1111) << 10) |
                ((z & 0b11'1111'1111) << 20) | ((w & 0b11) << 30)} {}
};
} // namespace detail

struct half_float {};
struct fixed {};

struct int_2_10_10_10_rev : detail::gen_2_10_10_10_rev {
  using gen_2_10_10_10_rev::gen_2_10_10_10_rev;

  constexpr static int_2_10_10_10_rev from_normal(const glm::vec3 v) {
    constexpr float MAX = (1 << 9) - 1;
    const glm::ivec3 i = v * MAX;
    const auto [x, y, z] = static_cast<glm::uvec3>(i);
    return {x, y, z};
  }
};
struct uint_2_10_10_10_rev : detail::gen_2_10_10_10_rev {
  using gen_2_10_10_10_rev::gen_2_10_10_10_rev;

  constexpr static uint_2_10_10_10_rev from_normal(const glm::vec3 v) {
    constexpr float MAX = (1 << 10) - 1;
    const auto [x, y, z] = static_cast<glm::uvec3>(v * MAX);
    return {x, y, z};
  }
};
struct uint_10_11_11_rev {};

template <typename T> GLenum macroOf;
template <> inline constexpr GLenum macroOf<GLbyte> = GL_BYTE;
template <> inline constexpr GLenum macroOf<GLubyte> = GL_UNSIGNED_BYTE;
template <> inline constexpr GLenum macroOf<GLshort> = GL_SHORT;
template <> inline constexpr GLenum macroOf<GLushort> = GL_UNSIGNED_SHORT;
template <> inline constexpr GLenum macroOf<GLint> = GL_INT;
template <> inline constexpr GLenum macroOf<GLuint> = GL_UNSIGNED_INT;
template <> inline constexpr GLenum macroOf<GLfloat> = GL_FLOAT;
template <> inline constexpr GLenum macroOf<GLdouble> = GL_DOUBLE;
template <>
inline constexpr GLenum macroOf<int_2_10_10_10_rev> = GL_INT_2_10_10_10_REV;
template <>
inline constexpr GLenum macroOf<uint_2_10_10_10_rev> =
    GL_UNSIGNED_INT_2_10_10_10_REV;
template <>
inline constexpr GLenum macroOf<uint_10_11_11_rev> =
    GL_UNSIGNED_INT_10F_11F_11F_REV;

} // namespace GL
