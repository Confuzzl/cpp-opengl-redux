#pragma once

#include "glad/gl.h"
#include <glm/glm.hpp>

#include <functional>
#include <string_view>

namespace GL {
struct IDHolder {
  GLuint ID;

  IDHolder(const GLuint ID) : ID{ID} {}
  IDHolder(const IDHolder &) = delete;
  IDHolder(IDHolder &&o) : ID{o.ID} { o.ID = 0; }
  IDHolder &operator=(const IDHolder &) = delete;
  IDHolder &operator=(IDHolder &&o) {
    ID = o.ID;
    o.ID = 0;
    return *this;
  }
};

std::string_view getErrorName();

std::string_view uniformTypeToString(const GLenum type);

template <typename T> GLenum uniformTypeMacro;
template <> constexpr inline GLenum uniformTypeMacro<float> = GL_FLOAT;
template <> constexpr inline GLenum uniformTypeMacro<glm::vec2> = GL_FLOAT_VEC2;
template <> constexpr inline GLenum uniformTypeMacro<glm::vec3> = GL_FLOAT_VEC3;
template <> constexpr inline GLenum uniformTypeMacro<glm::vec4> = GL_FLOAT_VEC4;
// template <> constexpr inline GLenum uniformTypeMacro<double> = GL_DOUBLE;
// template <>
// constexpr inline GLenum uniformTypeMacro<glm::dvec2> = GL_DOUBLE_VEC2;
// template <>
// constexpr inline GLenum uniformTypeMacro<glm::dvec3> = GL_DOUBLE_VEC3;
// template <>
// constexpr inline GLenum uniformTypeMacro<glm::dvec4> = GL_DOUBLE_VEC4;
template <> constexpr inline GLenum uniformTypeMacro<int> = GL_INT;
template <> constexpr inline GLenum uniformTypeMacro<glm::ivec2> = GL_INT_VEC2;
template <> constexpr inline GLenum uniformTypeMacro<glm::ivec3> = GL_INT_VEC3;
template <> constexpr inline GLenum uniformTypeMacro<glm::ivec4> = GL_INT_VEC4;
template <>
constexpr inline GLenum uniformTypeMacro<unsigned int> = GL_UNSIGNED_INT;
template <>
constexpr inline GLenum uniformTypeMacro<glm::uvec2> = GL_UNSIGNED_INT_VEC2;
template <>
constexpr inline GLenum uniformTypeMacro<glm::uvec3> = GL_UNSIGNED_INT_VEC3;
template <>
constexpr inline GLenum uniformTypeMacro<glm::uvec4> = GL_UNSIGNED_INT_VEC4;
// template <> constexpr inline GLenum uniformTypeMacro<bool> = GL_BOOL;
template <> constexpr inline GLenum uniformTypeMacro<glm::bvec2> = GL_BOOL_VEC2;
template <> constexpr inline GLenum uniformTypeMacro<glm::bvec3> = GL_BOOL_VEC3;
template <> constexpr inline GLenum uniformTypeMacro<glm::bvec4> = GL_BOOL_VEC4;
template <> constexpr inline GLenum uniformTypeMacro<glm::mat2> = GL_FLOAT_MAT2;
template <> constexpr inline GLenum uniformTypeMacro<glm::mat3> = GL_FLOAT_MAT3;
template <> constexpr inline GLenum uniformTypeMacro<glm::mat4> = GL_FLOAT_MAT4;
template <>
constexpr inline GLenum uniformTypeMacro<glm::mat2x3> = GL_FLOAT_MAT2x3;
template <>
constexpr inline GLenum uniformTypeMacro<glm::mat2x4> = GL_FLOAT_MAT2x4;
template <>
constexpr inline GLenum uniformTypeMacro<glm::mat3x2> = GL_FLOAT_MAT3x2;
template <>
constexpr inline GLenum uniformTypeMacro<glm::mat3x4> = GL_FLOAT_MAT3x4;
template <>
constexpr inline GLenum uniformTypeMacro<glm::mat4x2> = GL_FLOAT_MAT4x2;
template <>
constexpr inline GLenum uniformTypeMacro<glm::mat4x3> = GL_FLOAT_MAT4x3;

constexpr glm::u16vec2 uv_as_short(const glm::vec2 uv) {
  constexpr auto MAX = static_cast<GLushort>(-1);
  return {uv.x * MAX, uv.y * MAX};
}

template <typename T> GLenum elementTypeMacro;
template <>
constexpr inline GLenum elementTypeMacro<unsigned char> = GL_UNSIGNED_BYTE;
template <>
constexpr inline GLenum elementTypeMacro<unsigned short> = GL_UNSIGNED_SHORT;
template <>
constexpr inline GLenum elementTypeMacro<unsigned int> = GL_UNSIGNED_INT;

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

template <typename T> GLenum attribTypeMacro;
template <> inline constexpr GLenum attribTypeMacro<GLbyte> = GL_BYTE;
template <> inline constexpr GLenum attribTypeMacro<GLubyte> = GL_UNSIGNED_BYTE;
template <> inline constexpr GLenum attribTypeMacro<GLshort> = GL_SHORT;
template <>
inline constexpr GLenum attribTypeMacro<GLushort> = GL_UNSIGNED_SHORT;
template <> inline constexpr GLenum attribTypeMacro<GLint> = GL_INT;
template <> inline constexpr GLenum attribTypeMacro<GLuint> = GL_UNSIGNED_INT;
template <> inline constexpr GLenum attribTypeMacro<GLfloat> = GL_FLOAT;
template <> inline constexpr GLenum attribTypeMacro<GLdouble> = GL_DOUBLE;
template <>
inline constexpr GLenum attribTypeMacro<int_2_10_10_10_rev> =
    GL_INT_2_10_10_10_REV;
template <>
inline constexpr GLenum attribTypeMacro<uint_2_10_10_10_rev> =
    GL_UNSIGNED_INT_2_10_10_10_REV;
template <>
inline constexpr GLenum attribTypeMacro<uint_10_11_11_rev> =
    GL_UNSIGNED_INT_10F_11F_11F_REV;

struct DrawModifier {
  std::function<void()> start, reset;

  template <typename F1, typename F2>
  DrawModifier(F1 &&f1, F2 &&f2)
      : start{std::forward<F1>(f1)}, reset{std::forward<F2>(f2)} {}

  ~DrawModifier() { reset(); }

  template <typename F> static DrawModifier GLEnable(const GLenum cap, F &&f) {
    return DrawModifier{[cap, f]() {
                          glEnable(cap);
                          f();
                        },
                        [cap]() { glDisable(cap); }};
  }

  static DrawModifier primitiveRestart(const GLuint index) {
    return GLEnable(GL_PRIMITIVE_RESTART,
                    [index]() { glPrimitiveRestartIndex(index); });
  };
  static DrawModifier polygonMode(const GLenum face, const GLenum mode) {
    return DrawModifier{[face, mode]() { glPolygonMode(face, mode); },
                        [face]() { glPolygonMode(face, GL_FILL); }};
  }
};
} // namespace GL