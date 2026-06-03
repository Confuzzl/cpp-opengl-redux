#include "geom/obj_parser.h"

#include <fstream>
#include <sstream>
#include <string>

#include "util.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

ObjFile ObjParser::parse(const std::string_view name) {
  ObjFile out{};

  std::ifstream file{SOURCE_DIR "/assets/models/" + std::string{name}};
  if (!file)
    throw std::runtime_error{fmt::format("unable to open {}", name)};

  std::string line;
  while (std::getline(file, line)) {
    // fmt::println("line: \"{}\"", line);

    std::istringstream stream{line};
    std::string first;
    if (!(stream >> first))
      continue;

    if (first == "#") {
      continue;
    } else if (first == "v") {
      glm::vec4 params;
      params.w = 1;
      stream >> params.x >> params.y >> params.z >> params.w;

      const glm::vec3 point = params / params.w;

      out.geometries.emplace_back(point);
      // fmt::println("geom {} = {}", out.geometries.size(),
      //              glm::to_string(point));
    } else if (first == "vt") {
      glm::vec3 params;
      params.y = params.z = 1;
      stream >> params.x >> params.y >> params.z;

      out.textures.emplace_back(params);
    } else if (first == "vn") {
      glm::vec3 params;
      stream >> params.x >> params.y >> params.z;

      out.normals.emplace_back(params);
    } else if (first == "f") {
      auto &face = out.faces.emplace_back();

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

  return out;
}
