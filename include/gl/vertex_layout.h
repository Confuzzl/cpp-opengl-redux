#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>

#include "util.h"

namespace shaders::vertex_layout {
namespace detail {
template <typename... Ts> struct type_list {
  // static constexpr auto SIZE = sizeof...(Ts);
};

template <typename List, std::size_t I> struct subscript {};
template <typename Head, typename... Tail>
struct subscript<type_list<Head, Tail...>, 0> {
  using type = Head;
};
template <typename Head, typename... Tail, std::size_t I>
struct subscript<type_list<Head, Tail...>, I> {
  using type = typename subscript<type_list<Tail...>, I - 1>::type;
};

template <typename Target, typename List> struct contains {};
template <typename Target, typename Head>
struct contains<Target, type_list<Head>> {
  static constexpr bool value = std::same_as<Target, Head>;
};
template <typename Target, typename Head, typename... Tail>
struct contains<Target, type_list<Head, Tail...>> {
  static constexpr bool value =
      std::same_as<Target, Head> || contains<Target, type_list<Tail...>>::value;
};

template <typename T> struct Underlying {
  using Type = T;
  static constexpr auto length = 1;
};
template <glm::length_t L, typename T> struct Underlying<glm::vec<L, T>> {
  using Type = T;
  static constexpr auto length = L;
};
template <> struct Underlying<GL::int_2_10_10_10_rev> {
  using Type = GL::int_2_10_10_10_rev;
  static constexpr auto length = 4;
};
template <> struct Underlying<GL::uint_2_10_10_10_rev> {
  using Type = GL::uint_2_10_10_10_rev;
  static constexpr auto length = 4;
};
template <> struct Underlying<GL::uint_10_11_11_rev> {
  using Type = GL::uint_10_11_11_rev;
  static constexpr auto length = 3;
};

template <typename T, bool normalize = false> struct AttrF {
  using underlying = Underlying<T>::Type;
  using supported_types =
      type_list<GLfloat, GLdouble, GLbyte, GLubyte, GLshort, GLushort, GLint,
                GLuint, GL::int_2_10_10_10_rev, GL::uint_2_10_10_10_rev,
                GL::uint_10_11_11_rev>; // missing half and fixed
  static_assert(contains<underlying, supported_types>::value,
                "unsupported type for glVertexAttribPointer");
  using type = T;

  template <GLint size, GLenum type>
  static void format(const GLuint vaoID, const GLuint attrIndex,
                     const GLint offset) {
    glVertexArrayAttribFormat(vaoID, attrIndex, size, type, normalize, offset);
  }
};
template <typename T> struct AttrI {
  using underlying = Underlying<T>::Type;
  using supported_types =
      type_list<GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint>;
  static_assert(contains<underlying, supported_types>::value,
                "unsupported type for glVertexAttribIPointer");
  using type = T;

  template <GLint size, GLenum type>
  static void format(const GLuint vaoID, const GLuint attrIndex,
                     const GLint offset) {
    glVertexArrayAttribIFormat(vaoID, attrIndex, size, type, offset);
  }
};
template <typename T> struct AttrL {
  using underlying = Underlying<T>::Type;
  using supported_types = type_list<double>;
  static_assert(contains<underlying, supported_types>::value,
                "unsupported type of glVertexAttribLPointer");
  using type = T;

  template <GLint size, GLenum type>
  static void format(const GLuint vaoID, const GLuint attrIndex,
                     const GLint offset) {
    glVertexArrayAttribLFormat(vaoID, attrIndex, size, type, offset);
  }
};

template <typename Attr>
void enable_single(const GLuint vaoID, const GLuint attrIndex,
                   const GLint offset) {
  using underlying = Underlying<typename Attr::type>;
  using value_type = underlying::Type;
  glEnableVertexArrayAttrib(vaoID, attrIndex);
  Attr::format<underlying::length, GL::macroOf<value_type>>(vaoID, attrIndex,
                                                            offset);
  glVertexArrayAttribBinding(vaoID, attrIndex, 0);
}

template <typename... Attrs> struct Layout {
  // private:
  //  using list = detail::type_list<Attrs...>;

  static_assert(((std::is_trivially_destructible_v<Attrs>) && ...),
                "types should be trivially destructible");

  static constexpr auto DATA = ([]() {
    constexpr auto roundUp = [](const auto x, const auto step) constexpr {
      const auto rem = x % step;
      return rem == 0 ? x : x + step - rem;
    };
    using offset_array = std::array<std::size_t, sizeof...(Attrs)>;
    offset_array offsets{};
    std::size_t out = 0;
    std::size_t size, align;
    int i = 0;
    ((size = sizeof(typename Attrs::type),
      align = alignof(typename Attrs::type),
      out = (offsets[i++] = roundUp(out, align)) + size),
     ...);
    return std::make_pair(out, std::move(offsets));
  })();
  static constexpr auto SIZE = DATA.first;
  static constexpr auto OFFSETS = DATA.second;

  alignas(4) std::byte storage[SIZE];

public:
  constexpr Layout() = default;
  // Layout(const Attrs &...args) {
  //   int i = 0;
  //   // placement new constexpr in c++ 26
  //   ((*(new (storage + OFFSETS[i++]) Attrs) = args), ...);
  // }
  constexpr Layout(const Attrs::type &...args) {
    constexpr auto constexpr_memcpy = [](std::byte *dest, const auto x) {
      constexpr auto size = sizeof(x);
      using arr = std::array<std::byte, size>;
      const arr storage = std::bit_cast<arr>(x);
      for (std::size_t i = 0; i < size; i++) {
        dest[i] = storage[i];
      }
    };
    int i = 0;
    (constexpr_memcpy(storage + OFFSETS[i++], args), ...);
  }

  // template <std::size_t I> auto &get() {
  //   static_assert(I < SIZE);
  //   using Type = detail::subscript<list, I>::type;
  //   return *std::launder(reinterpret_cast<Type *>(storage + OFFSETS[I]));
  // }

  static void enable(const GLuint vaoID) {
    GLuint attrIndex = 0;
    ((enable_single<Attrs>(vaoID, attrIndex,
                           static_cast<GLint>(OFFSETS[attrIndex])),
      attrIndex++),
     ...);
  }
};
} // namespace detail

using detail::Layout, detail::AttrF, detail::AttrI, detail::AttrL;

using pos = Layout<AttrF<glm::vec3>>;

using postex = Layout<AttrF<glm::vec3>, AttrF<glm::vec2>>;

using postex2 = Layout<AttrF<glm::vec3>, AttrF<GL::uint_10_11_11_rev, true>>;

// using posnorm = Layout<AttrF<glm::vec3>, AttrF<glm::vec3>>;
using posnorm = Layout<AttrF<glm::vec3>, AttrF<GL::int_2_10_10_10_rev, true>>;

// using postexnorm = Layout<AttrF<glm::vec3>, AttrF<GL::uint_10_11_11_rev,
// true>,
//                           AttrF<GL::int_2_10_10_10_rev, true>>;

using sphere = Layout<AttrF<glm::vec3>, AttrF<float>>;

using cylinder = Layout<AttrF<glm::vec3>, AttrF<float>, AttrF<float>>;

using cone = cylinder;

using torus = cylinder;

using superquadric =
    Layout<AttrF<glm::vec3>, AttrF<glm::vec3>, AttrF<glm::vec3>>;

using normalmap = Layout<AttrF<glm::vec3>, AttrF<glm::vec3>, AttrF<glm::vec3>,
                         AttrF<glm::vec2>>;

using align = Layout<AttrF<glm::vec3>, AttrI<GLbyte>, AttrI<GLbyte>>;

// using posnorm2 = Layout<AttrF<glm::vec3>, AttrF<GL::int_2_10_10_10_rev,
// true>>;
} // namespace shaders::vertex_layout

namespace vert_lay = shaders::vertex_layout;