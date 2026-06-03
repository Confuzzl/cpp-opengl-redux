#pragma once

#include <vector>

#include "app/base_renderer.h"
#include "app/base_scene.h"
#include "app/base_world.h"
#include "geom/shape.h"

namespace scene {
struct World : BaseWorld {
  std::vector<Object> objects{};

  Object test;

  World();

  void update(const float dt) override;
};
struct Renderer : BaseRenderer {
  World *world;
  Renderer(World *const world) : BaseRenderer(world), world{world} {}

private:
  void renderImpl(const float dt) const override;
  void renderSidebar(const float dt) override;
};

struct Scene : BaseScene {
  World *world; // shadows
  Scene();
};
} // namespace scene