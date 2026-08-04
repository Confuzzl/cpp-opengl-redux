#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "app/base_scene.h"
#include "gl/glsl_object.h"
#include "input/input_handler.h"

#include <memory>

struct Initializer {
  GLFWwindow *window = nullptr;
  static constexpr glm::ivec2 DEFAULT_DIMENSIONS{1280, 720};
  float windowScale;
  glm::ivec2 framebufferSize;
  glm::ivec2 windowSize;

  InputHandler input;

  glm::mat4 projection;

  Initializer();
  ~Initializer();
};
struct App : Initializer {
  struct {
    shaders::test::Basic basic;
    shaders::test::Texcol texcol;
    shaders::test::Flat flat;
    shaders::test::Normal normal;
    shaders::test::Phong phong;
    shaders::test::Phong2 phong2;
    shaders::test::Sphere sphere;
    shaders::test::Cylinder cylinder;
    shaders::test::Cone cone;
    shaders::test::Torus torus;
    shaders::test::Superquadric superquadric;
    shaders::test::NormalMap normalMap;
    shaders::test::Align align;
    shaders::test::Texcol2 texcol2;
  } shaders;

  std::unique_ptr<BaseScene> scenes[8];
  BaseScene *activeScene;

  App();

  void run();
  void close();
};

App &app();
