#pragma once

#include "gl/vertex_layout.h"

#define NEW_VERT(tname, layout, sname)                                         \
  struct tname {                                                               \
    static constexpr char name[] = sname;                                      \
    using LayoutType = layout;                                                 \
  }

namespace shaders::vert {
NEW_VERT(Basic, vert_lay::pos, "basic.vert");
NEW_VERT(Sphere, vert_lay::sphere, "sphere.vert");
NEW_VERT(Cylinder, vert_lay::cylinder, "cylinder.vert");
NEW_VERT(Cone, vert_lay::cone, "cone.vert");
NEW_VERT(Torus, vert_lay::torus, "torus.vert");
NEW_VERT(Superquadric, vert_lay::superquadric, "superquadric.vert");
NEW_VERT(Normal, vert_lay::posnorm, "normal.vert");
NEW_VERT(Tex, vert_lay::postex, "tex.vert");
NEW_VERT(Tex2, vert_lay::postex2, "tex.vert");
NEW_VERT(Texnorm, vert_lay::postexnorm, "texnorm.vert");
NEW_VERT(NormalMap, vert_lay::normalmap, "normal_map.vert");
NEW_VERT(Align, vert_lay::align, "align.vert");
} // namespace shaders::vert

#undef NEW_VERT