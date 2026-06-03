#pragma once

#include "app/base_world.h"

struct BaseRenderer {
  BaseWorld *world;

  BaseRenderer(BaseWorld *const world) : world{world} {}

  void render(const float dt);

protected:
  void renderSidebarPrelude(const float dt) const;
  virtual void renderSidebar(const float dt) { (void)dt; }
  void renderSidebarPostlude() const;
  virtual void renderImpl(const float dt) const;
};