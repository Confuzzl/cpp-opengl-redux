#pragma once

#define NEW_FRAG(tname, sname)                                                 \
  struct tname {                                                               \
    static constexpr char name[] = sname;                                      \
  }

namespace shaders::frag {
NEW_FRAG(Basic, "basic.frag");
NEW_FRAG(Texcol, "texcol.frag");
NEW_FRAG(Flat, "flat.frag");
NEW_FRAG(Normal, "normal.frag");
NEW_FRAG(Phong, "phong.frag");
NEW_FRAG(Phong2, "phong2.frag");
NEW_FRAG(NormalMap, "normal_map.frag");
} // namespace shaders::frag

#undef NEW_FRAG