#pragma once

#include <glm/glm.hpp>

#include <string_view>
#include <vector>

struct ObjFile {
  std::vector<glm::vec3> geometries{}, textures{}, normals{};

  struct VertexIndex {
    unsigned int from1;
    VertexIndex(const unsigned int i) : from1{i} {}

    unsigned int index() const { return from1 - 1; }
    bool valid() const { return from1 != 0; }
    operator bool() const { return valid(); }
  };
  struct IndexTriplet {
    VertexIndex coordinate = 0, texture = 0, normal = 0;
  };
  struct Face {
    std::vector<IndexTriplet> indices{};
  };
  std::vector<Face> faces;
};

struct ObjParser {
  static ObjFile parse(const std::string_view name);
};