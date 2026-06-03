#pragma once

#pragma once

#include "gl/component/uniform_macros.h"

#define GEOM_BEGIN(tname, sname)                                               \
  struct tname : UniformHolder {                                               \
    static constexpr char name[] = sname
#define GEOM_END                                                               \
  using UniformHolder::UniformHolder;                                          \
  }

namespace shaders::geom {
// GEOM_BEGIN(Subdivide, "subdivide.tesc");
// NEW_UNIFORM(int, level);
// NEW_UNIFORM(int, face_normal);
// NEW_UNIFORM_BLOCK(ProjectionBlock);
// NEW_UNIFORM(glm::mat4, cam);
// NEW_UNIFORM(glm::mat4, model);
// GEOM_END;
} // namespace shaders::geom

#undef GEOM_BEGIN
#undef GEOM_END