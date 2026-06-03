#include "input/key.h"

#include <GLFW/glfw3.h>

static constexpr auto EMPTY = [](const float) {};

Key::Key(const Callback justOn, const Callback holdOn, const Callback justOff,
         const int mods)
    : justOn{justOn}, holdOn{holdOn}, justOff{justOff}, mods{mods} {}
// Key::Key(const Callback on, const Callback off)
//     : justOn{on}, holdOn{on}, justOff{off} {}
Key::Key(Hold, const Callback holdOn, const int mods)
    : Key(EMPTY, holdOn, EMPTY, mods) {}
Key::Key(Just, const Callback justOn, const int mods)
    : Key(justOn, EMPTY, EMPTY, mods) {}

void Key::react(const int action, const int mods) {
  if (this->mods != -1 && this->mods != mods)
    return;
  switch (action) {
  case GLFW_RELEASE: {
    on = false;
    justUsed = true;
    break;
  }
  case GLFW_PRESS: {
    on = true;
    justUsed = true;
    break;
  }
  case GLFW_REPEAT: {
  }
  }
}
void Key::reset() {
  on = false;
  justUsed = false;
}

void Key::operator()(const float dt) {
  if (on) {
    if (justUsed) {
      justOn(dt);
      justUsed = false;
    } else {
      holdOn(dt);
    }
  } else {
    if (justUsed) {
      justOff(dt);
      justUsed = false;
    }
  }
}