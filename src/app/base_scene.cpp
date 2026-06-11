#include "app/base_scene.h"

InputHandler BaseScene::getDefaultInput()
{
  return {
      {
          {GLFW_KEY_W, Key{Key::HOLD,
                           [this](const float dt)
                           {
                             world->cam.move(+world->cam.flatFront, dt);
                           }}},
          {GLFW_KEY_A, Key{Key::HOLD,
                           [this](const float dt)
                           {
                             world->cam.move(-world->cam.right, dt);
                           }}},
          {GLFW_KEY_S, Key{Key::HOLD,
                           [this](const float dt)
                           {
                             world->cam.move(-world->cam.flatFront, dt);
                           }}},
          {GLFW_KEY_D, Key{Key::HOLD,
                           [this](const float dt)
                           {
                             world->cam.move(+world->cam.right, dt);
                           }}},
          {GLFW_KEY_SPACE,
           Key{Key::HOLD,
               [this](const float dt)
               { world->cam.move(+Camera::UP, dt); }}},
          {GLFW_KEY_LEFT_CONTROL,
           Key{Key::HOLD,
               [this](const float dt)
               { world->cam.move(-Camera::UP, dt); }}},
          {GLFW_KEY_UP,
           Key{Key::HOLD,
               [this](const float dt)
               { world->cam.rotate(0, +100 * dt); }}},
          {GLFW_KEY_DOWN,
           Key{Key::HOLD,
               [this](const float dt)
               { world->cam.rotate(0, -100 * dt); }}},
          {GLFW_KEY_LEFT,
           Key{Key::HOLD,
               [this](const float dt)
               { world->cam.rotate(-100 * dt, 0); }}},
          {GLFW_KEY_RIGHT,
           Key{Key::HOLD,
               [this](const float dt)
               { world->cam.rotate(+100 * dt, 0); }}},
          {GLFW_KEY_X,
           Key{Key::JUST, [this](const float)
               { world->showAxes ^= true; }}},
          {GLFW_KEY_L,
           Key{Key::JUST,
               [this](const float)
               { world->light = world->cam.pos; }}},
          {GLFW_KEY_F1, Key{Key::JUST,
                            [this](const float)
                            {
                              world->renderState =
                                  BaseWorld::RenderState::WIREFRAME;
                            }}},
          {GLFW_KEY_F2, Key{Key::JUST,
                            [this](const float)
                            {
                              world->renderState = BaseWorld::RenderState::FLAT;
                            }}},
          {GLFW_KEY_F3, Key{Key::JUST,
                            [this](const float)
                            {
                              world->renderState =
                                  BaseWorld::RenderState::NORMAL;
                            }}},
          {GLFW_KEY_F4, Key{Key::JUST,
                            [this](const float)
                            {
                              world->renderState =
                                  BaseWorld::RenderState::PHONG;
                            }}},
      },
      []([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int button, [[maybe_unused]] int action, [[maybe_unused]] int mods) {},
      []([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] double xoffset, [[maybe_unused]] double yoffset) {},
      []([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] double xpos, [[maybe_unused]] double ypos) {},
  };
}

void BaseScene::addNewInputs(InputHandler &&extra)
{
  for (auto &&[i, k] : extra.keys)
  {
    input.keys.insert_or_assign(i, std::move(k));
  }
  input.mouseButtonCallback = extra.mouseButtonCallback;
  //[input](GLFWwindow *window, int button,
  //                                          int action, int mods) {
  //  input.mouseButtonCallback(window, button, action, mods);
  //};
  input.scrollCallback = extra.scrollCallback;
  //[input](GLFWwindow *window, double xoffset,
  //                                     double yoffset) {
  //  input.scrollCallback(window, xoffset, yoffset);
  //};
  input.cursorCallback = [this, callback = extra.cursorCallback](
                             GLFWwindow *window, double xpos, double ypos)
  {
    world->cam.rotate(Cursor::dGamePos.x, Cursor::dGamePos.y);
    callback(window, xpos, ypos);
  };
  //[this, input](GLFWwindow *window, double xpos,
  //                                           double ypos) {
  //  world->cam.rotate(Cursor::dGamePos.x, Cursor::dGamePos.y);
  //  input.cursorCallback(window, xpos, ypos);
  //};
}