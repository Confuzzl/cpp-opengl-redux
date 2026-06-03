#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include <map>

#include "input/key.h"

struct Cursor {
  static glm::vec2 windowPos, prevWindowPos, dWindowPos;
  static glm::vec2 gamePos, prevGamePos, dGamePos;

  static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
};

struct InputHandler {
  std::map<int, Key> keys;
  using MouseButtonCallback = std::function<void(GLFWwindow *, int, int, int)>;
  MouseButtonCallback mouseButtonCallback;
  using ScrollCallback = std::function<void(GLFWwindow *, double, double)>;
  ScrollCallback scrollCallback;
  using CursorCallback = std::function<void(GLFWwindow *, double, double)>;
  CursorCallback cursorCallback;

  InputHandler(std::map<int, Key> &&keys,
               MouseButtonCallback &&mouseButtonCallback,
               ScrollCallback &&scrollCallback, CursorCallback &&cursorCallback)
      : keys{std::move(keys)}, mouseButtonCallback{mouseButtonCallback},
        scrollCallback{scrollCallback}, cursorCallback{cursorCallback} {}
  InputHandler()
      : InputHandler(
            {}, [](GLFWwindow *, int, int, int) {},
            [](GLFWwindow *, double, double) {},
            [](GLFWwindow *, double, double) {}) {}

  void processKeys(const float dt);
  void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                   int mods);

  void reset();
  // void keyCallback(GLFWwindow *window, int key, int scancode, int action,
  //                  int mods);
  // void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
  // void mouseButtonCallback(GLFWwindow *window, int button, int action,
  //                          int mods);
  // void scrollCallback(GLFWwindow *window, double xpos, double ypos);
};