#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "color.h"
#include "geom/rotator.h"
#include "geom/triangle.h"

struct Shape {
  std::vector<Triangle> tris{};

  glm::mat4 transform{1.0f};

  Color color = RED;

  static Shape icosahedron() {
    Shape out{};
    out.tris.reserve(20);
    static constexpr glm::vec3 vertices[60]{
        {0.525731, 0, 0.850651},   {0, 0.850651, 0.525731},
        {-0.525731, 0, 0.850651},  {0, 0.850651, 0.525731},
        {-0.850651, 0.525731, 0},  {-0.525731, 0, 0.850651},
        {0, 0.850651, 0.525731},   {0, 0.850651, -0.525731},
        {-0.850651, 0.525731, 0},  {0.850651, 0.525731, 0},
        {0, 0.850651, -0.525731},  {0, 0.850651, 0.525731},
        {0.525731, 0, 0.850651},   {0.850651, 0.525731, 0},
        {0, 0.850651, 0.525731},   {0.525731, 0, 0.850651},
        {0.850651, -0.525731, 0},  {0.850651, 0.525731, 0},
        {0.850651, -0.525731, 0},  {0.525731, 0, -0.850651},
        {0.850651, 0.525731, 0},   {0.850651, 0.525731, 0},
        {0.525731, 0, -0.850651},  {0, 0.850651, -0.525731},
        {0.525731, 0, -0.850651},  {-0.525731, 0, -0.850651},
        {0, 0.850651, -0.525731},  {0.525731, 0, -0.850651},
        {0, -0.850651, -0.525731}, {-0.525731, 0, -0.850651},
        {0.525731, 0, -0.850651},  {0.850651, -0.525731, 0},
        {0, -0.850651, -0.525731}, {0.850651, -0.525731, 0},
        {0, -0.850651, 0.525731},  {0, -0.850651, -0.525731},
        {0, -0.850651, 0.525731},  {-0.850651, -0.525731, 0},
        {0, -0.850651, -0.525731}, {0, -0.850651, 0.525731},
        {-0.525731, 0, 0.850651},  {-0.850651, -0.525731, 0},
        {0, -0.850651, 0.525731},  {0.525731, 0, 0.850651},
        {-0.525731, 0, 0.850651},  {0.850651, -0.525731, 0},
        {0.525731, 0, 0.850651},   {0, -0.850651, 0.525731},
        {-0.850651, -0.525731, 0}, {-0.525731, 0, 0.850651},
        {-0.850651, 0.525731, 0},  {-0.525731, 0, -0.850651},
        {-0.850651, -0.525731, 0}, {-0.850651, 0.525731, 0},
        {0, 0.850651, -0.525731},  {-0.525731, 0, -0.850651},
        {-0.850651, 0.525731, 0},  {-0.850651, -0.525731, 0},
        {-0.525731, 0, -0.850651}, {0, -0.850651, -0.525731},
    };

    for (int i = 0; i < 60; i += 3) {
      out.tris.emplace_back(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    return out;
  }

  static Shape subdivideSphere(const Shape &shape) {
    Shape out;

    const auto oldSize = shape.tris.size();
    out.tris.reserve(oldSize * 4);
    for (std::size_t i = 0; i < oldSize; i++) {
      const auto &v = shape.tris[i].vertices;
      const auto v01 = glm::normalize((v[0] + v[1]) / 2.0f);
      const auto v12 = glm::normalize((v[1] + v[2]) / 2.0f);
      const auto v20 = glm::normalize((v[2] + v[0]) / 2.0f);
      out.tris.emplace_back(v[0], v01, v20);
      out.tris.emplace_back(v[1], v12, v01);
      out.tris.emplace_back(v[2], v20, v12);
      out.tris.emplace_back(v01, v12, v20);
    }
    out.transform = shape.transform;
    out.color = shape.color;

    return out;
  }
};

struct RotationController {
  Rotator rotator;
  glm::mat4 mat{1.0};
  bool rotate = false;

  void operator()(const float dt) {
    if (rotate)
      rotator(mat, dt);
  }
};

struct Object {
  const char *name;
  Shape shape;

  RotationController rotation;
};