#pragma once

#define NEW_GEOM(tname, sname)                                                 \
  struct tname {                                                               \
    static constexpr char name[] = sname;                                      \
  }

namespace shaders::geom {} // namespace shaders::geom

#undef NEW_GEOM