#pragma once

#include <glm/glm.hpp>

#include <set>
#include <vector>

#include "geom/obj_parser.h"
#include "runtime_array.h"

struct Vertex {
  unsigned int pos, norm, uv;
};
using VertexID = unsigned int;

struct Obj {
  runtime_array<glm::vec3> positions;
  runtime_array<glm::vec3> normals;
  runtime_array<glm::vec3> uvs;

  runtime_array<std::set<VertexID>> adjacency; // position adjacency

  struct Face {
    runtime_array<Vertex> vertices;
  };
  runtime_array<Face> faces;

  template <typename T> auto triangleFanIndices() const {
    // using T = int;
    using unsign = std::make_unsigned_t<T>;
    static constexpr auto RESTART = static_cast<unsign>(-1);

    std::vector<unsign> indices{};
    // runtime_array<unsign> indices{};
    unsign i = 0;
    for (const auto &face : faces) {
      for (const auto vertex : face.vertices) {
        indices.emplace_back(i++);
      }
      indices.emplace_back(RESTART);
    }

    return std::make_pair(std::move(indices), RESTART);
  }

  static Obj fromObjFile(const ObjFile &o);
};