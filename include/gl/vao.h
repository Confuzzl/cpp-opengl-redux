#pragma once

#include <glad/gl.h>

#include "gl/vertex_layout.h"

namespace GL {
template <typename VertexLayout> struct VertexArrayObject {
  GLuint ID;

  VertexArrayObject() {
    glCreateVertexArrays(1, &ID);
    //::shaders::vertex_layout::enable<VertexLayout>(ID);
    VertexLayout::enable(ID);
  }
  ~VertexArrayObject() { glDeleteVertexArrays(1, &ID); }
  VertexArrayObject(const VertexArrayObject &) = delete;
  VertexArrayObject(VertexArrayObject &&o) noexcept : ID{o.ID} { o.ID = 0; }
  VertexArrayObject &operator=(const VertexArrayObject &o) = delete;
  VertexArrayObject &operator=(VertexArrayObject &&o) noexcept {
    ID = o.ID;
    o.ID = 0;
    return *this;
  }
};
} // namespace GL