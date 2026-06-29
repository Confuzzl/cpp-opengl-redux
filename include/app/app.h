#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "app/base_scene.h"
#include "gl/gl_program.h"
#include "gl/glsl_object.h"
#include "gl/glsl_object2.h"
#include "input/input_handler.h"

#include <map>
#include <memory>
#include <string_view>

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
    shaders::Basic basic;
    shaders::Texcol texcol;
    shaders::Flat flat;
    shaders::Normal normal;
    shaders::Phong phong;
    // shaders::Phong2 phong2;
    shaders::Sphere sphere;
    shaders::Cylinder cylinder;
    shaders::Cone cone;
    shaders::Torus torus;
    shaders::Superquadric superquadric;
    shaders::NormalMap normalMap;
    shaders::Align align;
    shaders::Texcol2 texcol2;
  } shaders;
  struct {
    // shaders::test::Basic basic;
    // shaders::test::Texcol texcol;
    // shaders::test::Flat flat;
    // shaders::test::Normal normal;
    // shaders::test::Phong phong;
    shaders::test::Phong2 phong2;
    // shaders::test::Sphere sphere;
    // shaders::test::Cylinder cylinder;
    // shaders::test::Cone cone;
    // shaders::test::Torus torus;
    // shaders::test::Superquadric superquadric;
    // shaders::test::NormalMap normalMap;
    // shaders::test::Align align;
    // shaders::test::Texcol2 texcol2;
  } shaders2;

  std::unique_ptr<BaseScene> scenes[8];
  BaseScene *activeScene;

  App();

  void run();
  void close();
};

App &app();
