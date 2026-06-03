#pragma once

#include <array>
#include <glm/glm.hpp>

struct Triangle {
  std::array<glm::vec3, 3> vertices; // ccw
  glm::vec3 normal;

  Triangle(std::array<glm::vec3, 3> &&vertices);
  template <typename X, typename Y, typename Z>
  Triangle(X &&x, Y &&y, Z &&z)
      : Triangle(decltype(vertices){std::forward<X>(x), std::forward<Y>(y),
                                    std::forward<Z>(z)}) {}
};