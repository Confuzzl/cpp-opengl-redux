#include "geom/object.h"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <string_view>

Obj Obj::fromObjFile(const ObjFile::Object &o) {
  Obj out{};
  out.positions = {o.geometries.begin(), o.geometries.end()};
  out.normals = {o.normals.begin(), o.normals.end()};
  out.uvs = {o.textures.begin(), o.textures.end()};

  out.adjacency = {out.positions.size()};

  out.faces = {o.faces.size()};

  for (std::size_t i = 0; i < out.faces.size(); i++) {
    const auto &face = o.faces[i];
    const auto n = face.indices.size();
    out.faces[i].vertices = {n};
    for (std::size_t j = 0; j < n; j++) {
      const auto curr = face.indices[j], next = face.indices[(j + 1) % n];

      out.adjacency[curr.coordinate.index()].insert(next.coordinate.index());

      out.faces[i].vertices[j] = {.pos = curr.coordinate.index(),
                                  .norm = curr.normal.index(),
                                  .uv = curr.texture.index()};
    }
  }

  // for (int i = 0; i < out.adjacency.size(); i++) {
  //   fmt::println("{}: [{}]", i, fmt::join(out.adjacency[i], ","));
  // }

  return out;
}

Obj Obj::fromName(const std::string_view filename, const std::size_t i) {
  return fromObjFile(ObjParser::parse(filename).get(i));
}
Obj Obj::fromName(const std::string_view filename,
                  const std::string_view name) {
  return fromObjFile(ObjParser::parse(filename).get(name));
}