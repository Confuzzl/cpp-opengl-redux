#include "geom/obj_parser.h"

#include <fstream>
#include <optional>
#include <sstream>
#include <string>

#include "util.h"

#include <filesystem>
#include <glm/gtx/string_cast.hpp>

MtlFile MtlParser::parse(const std::string_view name) {
  std::ifstream file{SOURCE_DIR "/assets/models/" + std::string{name}};
  if (!file)
    throw std::runtime_error{fmt::format("unable to open {}", name)};

  MtlFile out{};
  out.name = name;

  MtlFile::Material *current = nullptr;

  std::string line;
  while (std::getline(file, line)) {
    fmt::println("line: \"{}\"", line);

    std::istringstream stream{line};
    std::string first;

    if (!(stream >> first))
      continue;

    if (first == "#")
      continue;
    else if (first == "newmtl") {
      std::string mtlName;
      stream >> mtlName;
      current = &out.materials.emplace_back(MtlFile::Material{.name = mtlName});
    } else if (first == "Ka") {
      glm::vec3 ambient;
      stream >> ambient.x >> ambient.y >> ambient.z;
      current->ambient = ambient;
    } else if (first == "Kd") {
      glm::vec3 diffuse;
      stream >> diffuse.x >> diffuse.y >> diffuse.z;
      current->diffuse = diffuse;
    } else if (first == "Ks") {
      glm::vec3 specular;
      stream >> specular.x >> specular.y >> specular.z;
      current->specularColor = specular;
    } else if (first == "Ns") {
      float specular;
      stream >> specular;
      current->specularExponent = specular;
    } else if (first == "map_Kd") {
      std::string texture;
      stream >> texture;
      current->diffuseTexture = texture;
    } else {
    }
  }

  for (const auto &mtl : out.materials) {
    fmt::println("{}: Ka={} Kd={} Ks={} Ns={} tex={}", mtl.name,
                 glm::to_string(mtl.ambient.value_or(glm::vec3{-1})),
                 glm::to_string(mtl.diffuse.value_or(glm::vec3{-1})),
                 glm::to_string(mtl.specularColor.value_or(glm::vec3{-1})),
                 mtl.specularExponent.value_or(-1),
                 mtl.diffuseTexture.value_or("no texture"));
  }

  return out;
}

ObjFile ObjParser::parse(const std::string_view name) {
  const std::filesystem::path path =
      SOURCE_DIR "/assets/models/" + std::string{name};
  std::ifstream file{path};
  if (!file)
    throw std::runtime_error{fmt::format("unable to open {}", name)};

  ObjFile out{};
  ObjFile::Object current{.name = path.stem().string()};

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream stream{line};
    std::string first;

    if (!(stream >> first))
      continue;

    if (first == "#") {
      continue;
    } else if (first == "mtllib") {
      std::string mtlFileName;
      stream >> mtlFileName;
      out.materialFiles.emplace_back(MtlParser::parse(mtlFileName));
    } else if (first == "usemtl") {
      std::string mtlName;
      stream >> mtlName;
      for (auto &mtlFile : out.materialFiles) {
        for (auto &mtl : mtlFile.materials) {
          if (mtl.name == mtlName) {
            fmt::println("found and using mtl {} from {}", mtl.name,
                         mtlFile.name);
            current.material = mtl;
          }
        }
      }
    } else if (first == "o") {
      out.addObject(std::move(current));
      current = {};
      stream >> current.name;
    } else if (first == "v") {
      glm::vec4 params;
      params.w = 1;
      stream >> params.x >> params.y >> params.z >> params.w;

      current.geometries.emplace_back(params / params.w);
    } else if (first == "vt") {
      glm::vec3 params;
      params.y = params.z = 1;
      stream >> params.x >> params.y >> params.z;

      current.textures.emplace_back(params);
    } else if (first == "vn") {
      glm::vec3 params;
      stream >> params.x >> params.y >> params.z;

      current.normals.emplace_back(params);
    } else if (first == "f") {
      auto &face = current.faces.emplace_back();

      std::string curr;
      while (stream >> curr) {
        glm::uvec3 index{0};
        std::istringstream indexStream{curr};
        std::string str;
        for (int j = 0; j < 3 && std::getline(indexStream, str, '/'); j++) {
          index[j] = (str == "") ? 0 : std::stoi(str);
        }

        face.indices.emplace_back(index.x, index.y, index.z);
      }
    } else {
    }
  }
  out.addObject(std::move(current));

  return out;
}
