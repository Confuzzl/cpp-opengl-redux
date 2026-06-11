#include "input/input_handler.h"

#include <imgui.h>

#include <algorithm>

#include "app/app.h"

glm::vec2 Cursor::windowPos{};
glm::vec2 Cursor::prevWindowPos{};
glm::vec2 Cursor::dWindowPos{};
glm::vec2 Cursor::gamePos{};
glm::vec2 Cursor::prevGamePos{};
glm::vec2 Cursor::dGamePos{};

static bool first = false;

void Cursor::cursorPosCallback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos)
{
  prevWindowPos = windowPos;
  prevGamePos = gamePos;
  const auto windowSize = app().windowSize;
  const auto y = ypos;
  // const auto y = std::clamp<double>(ypos, 0, windowSize.y);
  // const auto x = std::clamp<double>(xpos, 0, windowSize.x);
  const auto x = xpos;
  windowPos = {x * app().windowScale, y * app().windowScale};
  gamePos = {windowPos.x, (windowSize.y - y) * app().windowScale};
  if (!first)
  {
    first = true;
  }
  else
  {
    dWindowPos = windowPos - prevWindowPos;
    dGamePos = gamePos - prevGamePos;
  }
}

void InputHandler::processKeys(const float dt)
{
  for (auto &[keycode, key] : keys)
    key(dt);
}
void InputHandler::keyCallback([[maybe_unused]] GLFWwindow *window, int key, [[maybe_unused]] int scancode,
                               int action, int mods)
{
  if (const auto iterator = keys.find(key); iterator != keys.cend())
    iterator->second.react(action, mods);
}

void InputHandler::reset()
{
  for (auto &[keycode, key] : keys)
    key.reset();
}