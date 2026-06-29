#pragma once

#include <glm/glm.hpp>

#include <set>
#include <string_view>
#include <vector>

#include "color.h"
#include "geom/obj_parser.h"
#include "runtime_array.h"

struct Vertex {
  unsigned int pos, norm, uv;
};
using VertexID = unsigned int;

struct Material {
  std::string textureName;

  struct {
    Color color;
    float strength;
  } ambient, diffuse, specular;
  float shininess;

  Material();
};
struct Obj {
  Material material{};

  runtime_array<glm::vec3> positions{};
  runtime_array<glm::vec3> normals{};
  runtime_array<glm::vec3> uvs{};

  runtime_array<std::set<VertexID>> adjacency{}; // position adjacency

  struct Face {
    runtime_array<Vertex> vertices{};
  };
  runtime_array<Face> faces{};

  template <typename T> auto triangleFanIndices() const {
    using unsign = std::make_unsigned_t<T>;
    static constexpr auto RESTART = static_cast<unsign>(-1);

    std::vector<unsign> indices{};
    unsign i = 0;
    for (const auto &face : faces) {
      for (std::size_t j = 0; j < face.vertices.size(); j++) {
        indices.emplace_back(i++);
      }
      indices.emplace_back(RESTART);
    }

    return std::make_pair(std::move(indices), RESTART);
  }

  static Obj fromObjFile(const ObjFile::Object &o);
  static Obj fromName(const std::string_view filename, const std::size_t i = 0);
  static Obj fromName(const std::string_view filename,
                      const std::string_view n);
};