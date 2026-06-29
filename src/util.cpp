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