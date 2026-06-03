#include "geom/triangle.h"

Triangle::Triangle(std::array<glm::vec3, 3> &&vertices)
    : vertices{std::move(vertices)},
      normal{-glm::normalize(
          glm::cross(vertices[0] - vertices[1], vertices[2] - vertices[1]))} {}