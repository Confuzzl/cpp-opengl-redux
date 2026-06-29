#pragma once

#include <fmt/format.h>
#include <glm/glm.hpp>

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

struct MtlFile {
  struct Material {
    std::string name;

    std::optional<glm::vec3> ambient = std::nullopt;       // Ka
    std::optional<glm::vec3> diffuse = std::nullopt;       // Kd
    std::optional<glm::vec3> specularColor = std::nullopt; // Ks
    std::optional<float> specularExponent = std::nullopt;  // Ns

    std::optional<std::string> diffuseTexture = std::nullopt; // map_Kd
  };

  std::string name;
  std::vector<Material> materials{};

  Material &get(const std::size_t i) { return materials.at(i); }
  Material &get(const std::string_view name) {
    for (auto &mat : materials)
      if (mat.name == name)
        return mat;
    throw std::runtime_error{fmt::format("no material named {}", name)};
  }
};
struct MtlParser {
  static MtlFile parse(const std::string_view name);
};

struct ObjFile {
  struct Object {
    std::string name;

    MtlFile::Material material{};

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
    std::vector<Face> faces{};

    operator bool() const {
      return !(geometries.empty() && textures.empty() && normals.empty() &&
               faces.empty());
    }
  };

  void addObject(Object &&o) {
    if (o)
      objects.emplace_back(std::move(o));
  }

  std::vector<Object> objects{};
  std::vector<MtlFile> materialFiles{};

  Object &get(const std::size_t i) { return objects.at(i); }
  Object &get(const std::string_view name) {
    for (auto &obj : objects)
      if (obj.name == name)
        return obj;
    throw std::runtime_error{fmt::format("no object named {}", name)};
  }
};

struct ObjParser {
  static ObjFile parse(const std::string_view name);
};