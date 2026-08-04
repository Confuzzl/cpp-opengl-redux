#include "gl/uniform.h"

namespace shaders {
// UniformHolder::UniformHolder(const GLuint programID) : programID{programID}
// {}

namespace uniforms::detail {
GLuint getNewBinding() {
  static GLuint out = 0;
  return out++;
}
} // namespace uniforms::detail
} // namespace shaders

static GLuint getBinding(const GLuint shaderID, const char *name) {
  const GLint location = glGetUniformLocation(shaderID, name);
  if (location == -1)
    throw std::runtime_error{
        fmt::format("{}: {} was not a valid sampler name [{}]", shaderID, name,
                    GL::getErrorName())};
  GLuint binding;
  glGetUniformuiv(shaderID, location, &binding);
  // fmt::println("sampler {} at location={} binding={}", name, location,
  // binding);
  return binding;
}
namespace GL {
Sampler::Sampler(const GLuint programID, const char *name)
    : binding{getBinding(programID, name)} {}
} // namespace GL

// namespace shaders::detail {
// GLuint BINDINGS = 0;
// } // namespace shaders::detail