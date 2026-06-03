#pragma once

#include "gl/uniform.h"

#include "color.h"
#include "gl/component/uniform_macros.h"

#define FRAG_COLOR NEW_UNIFORM(Color, frag_color)
#define SAMPLER GL::Sampler sampler{programID, "sampler"}

#define FRAG_BEGIN(tname, sname)                                               \
  struct tname : UniformHolder {                                               \
    static constexpr char name[] = sname
#define FRAG_END                                                               \
  using UniformHolder::UniformHolder;                                          \
  }

namespace shaders::frag {
FRAG_BEGIN(Basic, "basic.frag");
FRAG_COLOR;
FRAG_END;

FRAG_BEGIN(Texcol, "texcol.frag");
NEW_SAMPLER(sampler);
FRAG_COLOR;
FRAG_END;

FRAG_BEGIN(Flat, "flat.frag");
FRAG_COLOR;
NEW_UNIFORM(Color, light_color);
NEW_UNIFORM(glm::vec3, light);
FRAG_END;

FRAG_BEGIN(Normal, "normal.frag");
FRAG_END;

FRAG_BEGIN(Phong, "phong.frag");
FRAG_COLOR;
NEW_UNIFORM(glm::vec3, light);
NEW_UNIFORM(Color, light_color);
NEW_UNIFORM(glm::vec3, camera_pos);
FRAG_END;

FRAG_BEGIN(NormalMap, "normal_map.frag");
NEW_SAMPLER(diffuse_map);
NEW_SAMPLER(normal_map);
// FRAG_COLOR;
NEW_UNIFORM(glm::vec3, light);
// NEW_UNIFORM(Color, light_color);
NEW_UNIFORM(glm::vec3, camera_pos);
FRAG_END;
} // namespace shaders::frag

#undef FRAG_COLOR
#undef SAMPLER
#undef FRAG_BEGIN
#undef FRAG_END