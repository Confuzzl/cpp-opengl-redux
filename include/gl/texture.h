#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <memory>
#include <string_view>

#include "color.h"
#include "gl_util.h"
#include "util.h"

namespace GL {
struct Texture : IDHolder {
  // struct STBIDeleter {
  //   void operator()(unsigned char *const data);
  // };

  // std::unique_ptr<unsigned char[], STBIDeleter> data;

  glm::uvec2 size;

  static constexpr GLenum DEFAULT_FILTER = GL_LINEAR, DEFAULT_WRAP = GL_REPEAT;

  Texture(const std::string_view name, const GLenum filter = DEFAULT_FILTER,
          const GLenum wrap = DEFAULT_WRAP);
  ~Texture();
  DEFAULT_MOVE_ONLY_TYPE(Texture);

  template <nttp_str NAME, GLenum filter = DEFAULT_FILTER,
            GLenum wrap = DEFAULT_WRAP>
  static GL::Texture &shared() {
    static GL::Texture out{NAME.m, filter, wrap};
    return out;
  }

  // Color at(const glm::uvec2 uv) const {
  //   const auto flat = (uv.y * size.x + uv.x) * 4;
  //   return {data[flat + 0], data[flat + 1], data[flat + 2], data[flat + 3]};
  // }
  // Color sample(const glm::vec2 uv) const;
};
} // namespace GL