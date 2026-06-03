#include "geom/rotator.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util.h"

Rotator::Rotator(const float axisSlowness, const float rotateSpeed)
    : axisSlowness{axisSlowness}, rotateSpeed{rotateSpeed},
      offset{random_float(0.0f, 1.0f)} {}

void Rotator::operator()(glm::mat4 &mat, const float dt) {
  static constexpr float PI = glm::pi<float>();

  timer += dt / axisSlowness;

  const float x = fmodf(timer, 2.0), y = fmodf(x + offset, 1.0);
  const float theta = x * PI, phi = y * 2 * PI;
  const glm::vec3 axis{
      sinf(theta) * cosf(phi),
      sinf(theta) * sinf(phi),
      cosf(theta),
  };

  mat = glm::rotate(mat, dt * rotateSpeed, axis);
}
