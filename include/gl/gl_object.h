#pragma once

#include <cstddef>
#include <glad/gl.h>
#include <initializer_list>
#include <utility>

#include "gl/vertex_layout.h"
#include "util.h"

namespace GL {
struct BufferObject {
  GLuint ID;
  GLsizeiptr size;
  GLsizei count = 0;

  BufferObject(const GLsizeiptr size, const GLbitfield flags,
               const void *data = nullptr);
  ~BufferObject();

  void reset() { count = 0; }
};

template <typename V = vert_lay::pos> struct VBO : BufferObject {
  static constexpr auto STRIDE = static_cast<GLsizei>(sizeof(V));

  VBO(const GLsizei count)
      : BufferObject(static_cast<GLsizeiptr>(count * STRIDE),
                     GL_DYNAMIC_STORAGE_BIT) {}
  template <typename T>
  VBO(const T &list)
      : BufferObject(static_cast<GLsizeiptr>(std::size(list) * STRIDE),
                     GL_DYNAMIC_STORAGE_BIT) {
    using value_type = std::decay_t<decltype(list[0])>;
    static_assert(std::is_same_v<V, value_type>);
    writeList(list);
  }

  void addCount(const GLsizei add, const void *data) {
    const GLsizei oldCount = count;
    const GLsizei newCount = count + add;

    const GLsizei newSize = newCount * STRIDE;
    if (newSize > size) {
      // print_info("RESIZING!");
      fmt::println("COUNTS {} -> {}", oldCount, newCount);
      fmt::println("RESIZING {} -> {}", size, newSize * 2);
      GLuint bigger;
      glCreateBuffers(1, &bigger);
      glNamedBufferStorage(bigger, newSize * 2, nullptr,
                           GL_DYNAMIC_STORAGE_BIT);
      glCopyNamedBufferSubData(ID, bigger, 0, 0, size);
      glDeleteBuffers(1, &ID);
      ID = bigger;
      size = newSize * 2;
    }
    glNamedBufferSubData(ID, oldCount * STRIDE, add * STRIDE, data);

    count = newCount;
  }

  void write(const V &val) { addCount(1, val.storage); }
  template <typename T> void writeList(const T &list, const bool reset = true) {
    using value_type = std::decay_t<decltype(list[0])>;
    static_assert(std::is_same_v<V, value_type>);

    if (reset)
      count = 0;

    addCount(std::size(list), std::data(list));
    // const auto lsize = std::size(list);
    // const GLsizei oldCount = count;
    // const GLsizei newCount = count + lsize;

    // const GLsizei newSize = newCount * STRIDE;
    // if (newSize > size) {
    //   fmt::println("COUNTS {} -> {}", oldCount, newCount);
    //   fmt::println("RESIZING {} -> {}", size, newSize * 2);
    //   GLuint bigger;
    //   glCreateBuffers(1, &bigger);
    //   glNamedBufferStorage(bigger, newSize * 2, nullptr,
    //                        GL_DYNAMIC_STORAGE_BIT);
    //   glCopyNamedBufferSubData(ID, bigger, 0, 0, size);
    //   glDeleteBuffers(1, &ID);
    //   ID = bigger;
    //   size = newSize * 2;
    // }
    // glNamedBufferSubData(ID, oldCount * STRIDE, lsize * STRIDE,
    //                      std::data(list));
    // count = newCount;
  }
};
struct EBO : BufferObject {
  GLenum type;

  template <typename T>
  EBO(const T &list) : EBO(std::data(list), std::size(list)) {}

  template <typename T>
  EBO(const T *const data, const std::size_t size)
      : BufferObject{static_cast<GLsizeiptr>(size * sizeof(T)),
                     GL_DYNAMIC_STORAGE_BIT, data},
        type{macroOf<std::make_unsigned_t<T>>} {
    count = static_cast<GLsizei>(size);
  }
};
} // namespace GL