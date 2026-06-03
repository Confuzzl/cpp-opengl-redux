#include "util.h"

#include <fmt/color.h>

#include <stdexcept>

glm::vec2 random_vec2() {
  const float theta = random_float(0.0f, 2 * 3.141592f);
  return {std::cos(theta), std::sin(theta)};
}

glm::mat3 normal_matrix(const glm::mat3 &mat) {
  return glm::mat3{glm::transpose(glm::inverse(mat))};
}

std::string_view GL::getErrorName() {
  switch (glGetError()) {
  case GL_NO_ERROR:
    return "NO ERROR";
  case GL_INVALID_ENUM:
    return "INVALID ENUM";
  case GL_INVALID_VALUE:
    return "INVALID VALUE";
  case GL_INVALID_OPERATION:
    return "INVALID OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "INVALID FRAMEBUFFER OPERATION";
  case GL_OUT_OF_MEMORY:
    return "OUT OF MEMORY";
  case GL_STACK_UNDERFLOW:
    return "STACK UNDERFLOW";
  case GL_STACK_OVERFLOW:
    return "STACK OVERFLOW";
  }
  throw std::runtime_error{"INVALID ERROR ENUM"};
}