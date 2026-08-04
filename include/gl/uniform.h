#pragma once

#include <fmt/base.h>
#include <fmt/format.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl/gl_object.h"
#include "util.h"

namespace GL {
template <typename T> struct UBO2 : BufferObject {
  T data{};
  GLuint binding;

  UBO2(const GLuint binding)
      : BufferObject(sizeof(T), GL_DYNAMIC_STORAGE_BIT), binding{binding} {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ID);

    fmt::println("created UBO {} {} binding {}", ID, typeid(T).name(), binding);
  }
  template <typename U = T> void update(U &&x) {
    data = T{std::forward<U>(x)};
    glNamedBufferSubData(ID, 0, sizeof(T), &data);
  }
};

struct Sampler {
  GLuint binding;

  Sampler(const GLuint programID, const char *name);
};
} // namespace GL

namespace shaders::uniforms {
struct alignas(16) ProjectionBlock {
  static constexpr char name[] = "ProjectionBlock";
  glm::mat4 proj;
};
struct alignas(16) CameraBlock {
  static constexpr char name[] = "CameraBlock";
  glm::mat4 cam;
};
struct alignas(16) PhongData {
  static constexpr char name[] = "PhongData";

  glm::vec3 pos;
  float shininess;

  unsigned int color;

  struct {
    unsigned int color;
    float strength;
  } ambient, diffuse, specular;
};

namespace detail {
GLuint getNewBinding();
} // namespace detail
template <typename T> auto &shared() {
  static GL::UBO2<T> out{detail::getNewBinding()};
  return out;
}
} // namespace shaders::uniforms