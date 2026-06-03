#pragma once

#include "gl/component/uniform_macros.h"

#define TESS_BEGIN(tname, psize, cname, ename)                                 \
  struct tname : UniformHolder {                                               \
    static constexpr unsigned int PATCH_SIZE = psize;                          \
    static constexpr char controlName[] = cname;                               \
    static constexpr char evalName[] = ename
#define TESS_END                                                               \
  using UniformHolder::UniformHolder;                                          \
  }

namespace shaders::tess {
TESS_BEGIN(Sphere, 1, "sphere.tesc", "sphere.tese");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
TESS_END;

TESS_BEGIN(Cylinder, 1, "cylinder.tesc", "cylinder.tese");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
TESS_END;

TESS_BEGIN(Cone, 1, "cone.tesc", "cone.tese");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
TESS_END;

TESS_BEGIN(Torus, 1, "torus.tesc", "torus.tese");
NEW_UNIFORM(float, resolution);
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
TESS_END;

TESS_BEGIN(Superquadric, 1, "superquadric.tesc", "superquadric.tese");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
TESS_END;
} // namespace shaders::tess

#undef TESS_BEGIN
#undef TESS_END