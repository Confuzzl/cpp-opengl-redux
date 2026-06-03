#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string_view>

#include "util.h"

namespace GL {
struct Texture {
  GLuint ID;
  glm::uvec2 size;

  Texture(const std::string_view name, const GLenum filter = GL_LINEAR,
          const GLenum wrap = GL_REPEAT);
  ~Texture();
  MOVE_ONLY_TYPE(Texture)

  template <nttp_str NAME, GLenum filter = GL_LINEAR>
  static GL::Texture &shared() {
    static GL::Texture out{NAME.m, filter};
    return out;
  }
};
} // namespace GL