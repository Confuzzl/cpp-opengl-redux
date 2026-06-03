#include "app/app.h"

#include <fmt/color.h>
#include <fmt/format.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdexcept>

#include "app/scene.h"
#include "callback.h"
#include "util.h"

template <int I> static constexpr std::pair<int, Key> sceneChangeCallback() {
  return {GLFW_KEY_0 + I, Key{Key::JUST, [](const float) {
                                app().activeScene->input.reset();
                                app().activeScene = app().scenes[I - 1].get();
                              }}};
}

static bool lockCursor = true;

Initializer::Initializer()
    : input{
          {
              {GLFW_KEY_ESCAPE,
               Key{Key::JUST, [](const float) { app().close(); }}},
              // sceneChangeCallback<1>(),
              // sceneChangeCallback<2>(),
              // sceneChangeCallback<3>(),
              // sceneChangeCallback<4>(),
              // sceneChangeCallback<5>(),
              // sceneChangeCallback<6>(),
              // sceneChangeCallback<7>(),
              // sceneChangeCallback<8>(),
              {GLFW_KEY_TAB,
               Key{
                   Key::JUST,
                   [this](const float) {
                     lockCursor ^= true;
                     glfwSetInputMode(window, GLFW_CURSOR,
                                      lockCursor ? GLFW_CURSOR_DISABLED
                                                 : GLFW_CURSOR_NORMAL);
                   },
               }},
          },
          [](GLFWwindow *window, int button, int action, int mods) {
            (void)window, button, action, mods;
          },
          [](GLFWwindow *window, double xoffset, double yoffset) {
            (void)window, xoffset, yoffset;
          },
          [](GLFWwindow *window, double xpos, double ypos) {
            (void)window, xpos, ypos;
          },
      },
      projection{} {
  if (!glfwInit()) {
    throw std::runtime_error{"GLFW FAILED TO INIT"};
  }

  glfwSetErrorCallback(callback::error);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_MAXIMIZED, true);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  window = glfwCreateWindow(DEFAULT_DIMENSIONS.x, DEFAULT_DIMENSIONS.y,
                            "Template", nullptr, nullptr);
  if (!window) {
    throw std::runtime_error{"GLFW FAILED TO CREATE WINDOW"};
  }
  // float main_scale =
  //     ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

  // fmt::println("scale = {}", main_scale);

  // glm::vec2 scale;
  // glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &scale.x, &scale.y);
  // fmt::println("scale2 = ({},{})", scale.x, scale.y);

  glfwGetFramebufferSize(window, &framebufferSize.x, &framebufferSize.y);
  glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

  // fmt::println("fb=({},{}) ws=({},{})", framebufferSize.x, framebufferSize.y,
  //              windowSize.x, windowSize.y);

  const auto xScale = static_cast<float>(framebufferSize.x) / windowSize.x,
             yScale = static_cast<float>(framebufferSize.y) / windowSize.y;
  if (xScale != yScale)
    throw std::runtime_error{"window scales didnt match"};
  fmt::println("windowscale={}", xScale);
  windowScale = xScale;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(static_cast<GLADloadfunc>(glfwGetProcAddress))) {
    throw std::runtime_error{"GLAD FAILED TO LOAD"};
  }

  glfwSetFramebufferSizeCallback(window, callback::framebufferSize);
  glfwSetWindowSizeCallback(window, callback::windowSize);

  glfwSetCursorPosCallback(
      window, [](GLFWwindow *window, double xpos, double ypos) {
        Cursor::cursorPosCallback(window, xpos, ypos);
        if (lockCursor) {
          app().input.cursorCallback(window, xpos, ypos);
          app().activeScene->input.cursorCallback(window, xpos, ypos);
        }
      });

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    app().input.keyCallback(window, key, scancode, action, mods);
    app().activeScene->input.keyCallback(window, key, scancode, action, mods);
  });
  glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button,
                                        int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse)
      return;
    app().input.mouseButtonCallback(window, button, action, mods);
    app().activeScene->input.mouseButtonCallback(window, button, action, mods);
  });
  glfwSetScrollCallback(
      window, [](GLFWwindow *window, double xpos, double ypos) {
        // if (ImGui::GetIO().WantCaptureMouse)
        //   return;
        app().input.scrollCallback(window, xpos, ypos);
        app().activeScene->input.scrollCallback(window, xpos, ypos);
      });
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(callback::debug, 0);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  // ImGuiIO &io = ImGui::GetIO();
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450");
  ImGui::StyleColorsDark();

  glPointSize(10.0f);
  glLineWidth(2.0f);
}
Initializer::~Initializer() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // glfwDestroyWindow(window);
  // glfwTerminate();
}

App::App()
    : scenes{
         std::make_unique<scene::Scene>(),
      }, activeScene{scenes[0].get()} {
}
void App::run() {
  glfwMaximizeWindow(window);

  // ImGuiIO &io = ImGui::GetIO();

  double prevT = glfwGetTime(), currT = prevT;
  while (!glfwWindowShouldClose(window)) {
    currT = glfwGetTime();
    const float dt = static_cast<float>(currT - prevT);

    glfwPollEvents();
    input.processKeys(dt);
    activeScene->input.processKeys(dt);

    activeScene->world->update(dt);

    const glm::vec4 color = activeScene->world->skyColor;
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    activeScene->renderer->render(dt);
    glfwSwapBuffers(window);

    prevT = currT;
  }
}
void App::close() { glfwSetWindowShouldClose(window, true); }

App &app() {
  static App instance{};
  return instance;
}
