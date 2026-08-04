#include "callback.h"

#include <fmt/color.h>
#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>

#include <string_view>

#include "app/app.h"
#include "gl/uniform.h"
#include "util.h"

namespace callback {
void error(int error_code, const char *const description) {
  print_err("GLFW ERROR ({}):\n{}", error_code, description);
}
void framebufferSize([[maybe_unused]] GLFWwindow *window, int width,
                     int height) {
  glViewport(0, 0, width, height);
  app().windowSize = {width, height};
  app().projection =
      glm::perspectiveFov(glm::radians(90.0f), static_cast<float>(width),
                          static_cast<float>(height), 0.1f, 500.f);
  {
    using namespace shaders::uniforms;
    shared<ProjectionBlock>().update(app().projection);
  }
}
void windowSize([[maybe_unused]] GLFWwindow *window, int width, int height) {
  app().windowSize = {width, height};
}

static std::string_view sourceName(const GLenum source) {
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    return "API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "WINDOW SYSTEM";
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "SHADER COMPILER";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "THIRD PARTY";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "APPLICATION";
  case GL_DEBUG_SOURCE_OTHER:
    return "OTHER";
  }
  throw std::runtime_error{"INVALID SOURCE ENUM"};
}
static std::string_view typeName(const GLenum type) {
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    return "ERROR";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "DEPRECATED BEHAVIOR";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "UNDEFINED BEHAVIOR";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "PORTABILITY";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "PERFORMANCE";
  case GL_DEBUG_TYPE_MARKER:
    return "MARKER";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "PUSH GROUP";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "POP GROUP";
  case GL_DEBUG_TYPE_OTHER:
    return "OTHER";
  }
  throw std::runtime_error{"INVALID TYPE ENUM"};
}
static std::string_view severityName(const GLenum severity) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    return "HIGH";
  case GL_DEBUG_SEVERITY_MEDIUM:
    return "MEDIUM";
  case GL_DEBUG_SEVERITY_LOW:
    return "LOW";
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    return "NOTIFICATION";
  }
  throw std::runtime_error{"INVALID SEVERITY ENUM"};
}
static fmt::color severityColor(const GLenum severity) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    return fmt::color::red;
  case GL_DEBUG_SEVERITY_MEDIUM:
    return fmt::color::orange;
  case GL_DEBUG_SEVERITY_LOW:
    return fmt::color::yellow;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    return fmt::color::sky_blue;
  }
  throw std::runtime_error{"INVALID SEVERITY ENUM"};
}
void APIENTRY debug(GLenum source, GLenum type, GLuint id, GLenum severity,
                    [[maybe_unused]] GLsizei length, const GLchar *message,
                    [[maybe_unused]] const void *userParam) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;
  const auto src = sourceName(source);
  const auto typ = typeName(type);
  const auto sev = severityName(severity);
  const fmt::color col = severityColor(severity);
  fmt::print(fmt::fg(col),
             "OPENGL DEBUG: source={} type={} id={} severity={}\n", src, typ,
             id, sev);
  fmt::println(fmt::runtime(message));
}
} // namespace callback