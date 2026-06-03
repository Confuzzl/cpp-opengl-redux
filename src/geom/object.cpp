#include "geom/object.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

Obj Obj::fromObjFile(const ObjFile &objFile) {
  Obj out{};
  out.positions = {objFile.geometries.begin(), objFile.geometries.end()};
  out.normals = {objFile.normals.begin(), objFile.normals.end()};
  out.uvs = {objFile.textures.begin(), objFile.textures.end()};

  out.adjacency = {out.positions.size()};

  out.faces = {objFile.faces.size()};

  for (int i = 0; i < out.faces.size(); i++) {
    const auto &face = objFile.faces[i];
    const auto n = face.indices.size();
    out.faces[i].vertices = {n};
    for (int j = 0; j < n; j++) {
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