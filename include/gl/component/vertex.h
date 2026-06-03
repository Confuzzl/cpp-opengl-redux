#pragma once

#include "gl/uniform.h"

#include "gl/component/uniform_macros.h"
#include "gl/vertex_layout.h"

#define VERT_BEGIN(tname, layout, sname)                                       \
  struct tname : Base<layout> {                                                \
    static constexpr char name[] = sname
#define VERT_END                                                               \
  using Base::Base;                                                            \
  }

namespace shaders::vert {
template <typename VertexLayout> struct Base : UniformHolder {
  using LayoutType = VertexLayout;

  using UniformHolder::UniformHolder;
};

VERT_BEGIN(Basic, vertex_layout::pos, "basic.vert");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
VERT_END;

VERT_BEGIN(Sphere, vertex_layout::sphere, "sphere.vert");
VERT_END;

VERT_BEGIN(Cylinder, vertex_layout::cylinder, "cylinder.vert");
VERT_END;

VERT_BEGIN(Cone, vertex_layout::cone, "cone.vert");
VERT_END;

VERT_BEGIN(Torus, vertex_layout::torus, "torus.vert");
VERT_END;

VERT_BEGIN(Superquadric, vertex_layout::superquadric, "superquadric.vert");
VERT_END;

VERT_BEGIN(Normal, vertex_layout::posnorm, "normal.vert");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
VERT_END;

VERT_BEGIN(Tex, vertex_layout::postex, "tex.vert");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
VERT_END;

VERT_BEGIN(Tex2, vertex_layout::postex2, "tex.vert");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
VERT_END;

VERT_BEGIN(NormalMap, vertex_layout::normalmap, "normal_map.vert");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
VERT_END;

VERT_BEGIN(Align, vertex_layout::align, "align.vert");
NEW_UNIFORM_BLOCK(ProjectionBlock);
NEW_UNIFORM_BLOCK(CameraBlock);
NEW_UNIFORM(glm::mat4, model);
VERT_END;

// VERT_BEGIN(Normal2, vertex_layout::posnorm2, "normal.vert");
// NEW_UNIFORM_BLOCK(ProjectionBlock);
// NEW_UNIFORM_BLOCK(CameraBlock);
// NEW_UNIFORM(glm::mat4, model);
// VERT_END;
} // namespace shaders::vert

#undef VERT_BEGIN
#undef VERT_END