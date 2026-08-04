#pragma once

#define NEW_TESS(tname, psize, cname, ename)                                   \
  struct tname {                                                               \
    static constexpr unsigned int PATCH_SIZE = psize;                          \
    static constexpr char controlName[] = cname;                               \
    static constexpr char evalName[] = ename;                                  \
  }

namespace shaders::tess {
NEW_TESS(Sphere, 1, "sphere.tesc", "sphere.tese");
NEW_TESS(Cylinder, 1, "cylinder.tesc", "cylinder.tese");
NEW_TESS(Cone, 1, "cone.tesc", "cone.tese");
NEW_TESS(Torus, 1, "torus.tesc", "torus.tese");
NEW_TESS(Superquadric, 1, "superquadric.tesc", "superquadric.tese");
} // namespace shaders::tess

#undef NEW_TESS