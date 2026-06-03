#pragma once

#include <glm/glm.hpp>

struct Rotator {
  float axisSlowness;
  float rotateSpeed;

private:
  float offset;
  float timer = 0;

public:
  Rotator(const float axisSlowness = 4, const float rotateSpeed = 1);

  void operator()(glm::mat4 &mat, const float dt);
};