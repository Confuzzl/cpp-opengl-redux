#include "app/scene.h"

#include "app/app.h"
#include "geom/obj_parser.h"
#include "geom/object.h"
#include "gl/gl_object.h"
#include "util.h"

#include <imgui.h>

#include "gl/bo_heap.h"

using namespace scene;

Scene::Scene()
    : BaseScene(std::in_place_type<World>, std::in_place_type<Renderer>,
                {
                    {},
                    [](GLFWwindow *window, int button, int action, int mods) {
                      (void)window, button, action, mods;
                    },
                    [](GLFWwindow *window, double xoffset, double yoffset) {
                      (void)window, xoffset, yoffset;
                    },
                    [](GLFWwindow *window, double xpos, double ypos) {
                      (void)window, xpos, ypos;
                    },
                }) {
  world = static_cast<World *>(BaseScene::world.get());
}

World::World() : BaseWorld() {
  {
    Shape tetrahedron;
    tetrahedron.tris.reserve(4);
    static constexpr glm::vec3 vertices[4]{
        {-1, -1, +1},
        {+1, -1, -1},
        {-1, +1, -1},
        {+1, +1, +1},
    };
    static constexpr int indices[4][3]{
        {0, 1, 3},
        {1, 0, 2},
        {0, 3, 2},
        {1, 2, 3},
    };
    for (int i = 0; i < 4; i++) {
      tetrahedron.tris.emplace_back(vertices[indices[i][0]],
                                    vertices[indices[i][1]],
                                    vertices[indices[i][2]]);
    }
    tetrahedron.color = LIGHT_RED;
    tetrahedron.transform = glm::translate(tetrahedron.transform, {-3, 0, 0});
    objects.emplace_back(Object{"Tetrahedron", tetrahedron, Rotator{}});
  }
  {
    Shape cube;
    cube.tris.reserve(12);
    static constexpr glm::vec3 vertices[8]{
        {-1, -1, +1}, {+1, -1, +1}, {+1, -1, -1}, {-1, -1, -1},
        {-1, +1, -1}, {+1, +1, -1}, {+1, +1, +1}, {-1, +1, +1},
    };
    static constexpr int indices[12][3]{
        {0, 2, 1}, {0, 3, 2}, {0, 1, 6}, {0, 6, 7}, {1, 2, 5}, {1, 5, 6},
        {2, 3, 4}, {2, 4, 5}, {3, 0, 7}, {3, 7, 4}, {7, 6, 5}, {7, 5, 4},
    };
    for (int i = 0; i < 12; i++) {
      cube.tris.emplace_back(vertices[indices[i][0]], vertices[indices[i][1]],
                             vertices[indices[i][2]]);
    }
    cube.color = LIGHT_BLUE;
    objects.emplace_back(Object{
        "Cube",
        cube,
        RotationController{Rotator{}},
    });

    cube.transform = glm::translate(cube.transform, {0, 2, 0});
    test = Object{
        "Test",
        cube,
        RotationController{Rotator{}},
    };
  }
  {
    Shape octahedron;
    octahedron.tris.reserve(8);
    static constexpr glm::vec3 vertices[6]{
        {0, -1.414213, 0}, {-1, 0, -1}, {+1, 0, -1},
        {+1, 0, +1},       {-1, 0, +1}, {0, +1.414213, 0},
    };
    static constexpr int indices[8][3]{
        {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 1},
        {5, 2, 1}, {5, 3, 2}, {5, 4, 3}, {5, 1, 4},
    };
    for (int i = 0; i < 8; i++) {
      octahedron.tris.emplace_back(vertices[indices[i][0]],
                                   vertices[indices[i][1]],
                                   vertices[indices[i][2]]);
    }
    octahedron.color = LIGHT_GREEN;
    octahedron.transform = glm::translate(octahedron.transform, {+3, 0, 0});
    objects.emplace_back(Object{
        "Octahedron",
        octahedron,
        RotationController{Rotator{}},
    });
  }
}

void World::update(const float dt) {
  for (auto &obj : objects)
    obj.rotation(dt);
}

void Renderer::renderImpl(const float dt) const {
  BaseRenderer::renderImpl(dt);

  static const auto obj =
      Obj::fromObjFile(ObjParser::parse("monkey_smooth.obj"));
  static const auto [list, restart] = obj.triangleFanIndices<int>();
  static GL::EBO TESTEBO{list};
  static GL::VBO<vert_lay::posnorm> TEST{1000};
  for (const auto &face : obj.faces) {
    for (const auto [c, n, t] : face.vertices) {
      TEST.write(vert_lay::posnorm{
          obj.positions[c],
          GL::int_2_10_10_10_rev::from_normal(obj.normals[n]),
      });
    }
  }
  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(restart);
  app()
      .shaders.phong.setModel(glm::translate(glm::mat4{1.0f}, {}))
      .setLight(world->light)
      .setLightColor(world->lightColor)
      .setCameraPos(world->cam.pos)
      .setFragColor(WHITE)
      .draw(GL_TRIANGLE_FAN, TEST, TESTEBO);
  glDisable(GL_PRIMITIVE_RESTART);

  return;

  if (world->renderState == World::RenderState::WIREFRAME) {
    static GL::VBO<> WIRE{6 * 2 * 3};
    for (const auto &[name, shape, rotation] : world->objects) {
      for (const auto &tri : shape.tris)
        for (const auto v : tri.vertices)
          WIRE.write({v});

      app()
          .shaders
          .basic
          //.setCamera(world->cam.matrix())
          .setModel(shape.transform * rotation.mat)
          .setFragColor(shape.color)
          .draw(GL_LINE_LOOP, WIRE);
    }
    return;
  }

  static GL::VBO<vert_lay::posnorm> NORM{6 * 2 * 3};
  for (const auto &[name, shape, rotation] : world->objects) {
    const auto t = shape.transform * rotation.mat;
    const auto nm = normal_matrix(t);
    for (const auto &tri : shape.tris) {
      const auto normal = nm * tri.normal;
      for (const auto v : tri.vertices)
        NORM.write(
            vert_lay::posnorm{v, GL::int_2_10_10_10_rev::from_normal(normal)});
    }

    switch (world->renderState) {
    case World::RenderState::FLAT:
      app()
          .shaders.flat.setModel(t)
          .setLight(world->light)
          .setLightColor(world->lightColor)
          .setFragColor(shape.color)
          .draw(GL_TRIANGLES, NORM);

      break;
    case World::RenderState::NORMAL:
      app().shaders.normal.setModel(t).draw(GL_TRIANGLES, NORM);
      break;
    case World::RenderState::PHONG:
      app()
          .shaders.phong.setModel(t)
          .setLight(world->light)
          .setLightColor(world->lightColor)
          .setCameraPos(world->cam.pos)
          .setFragColor(shape.color)
          .draw(GL_TRIANGLES, NORM);
      break;
    }
  }

  // static GL::VBO<vert_lay::align> VBO{1};
  // VBO.write(vert_lay::align{{0, 2, 0}, 127, -128});
  // app().shaders.align.setModel({1.0f}).setFragColor(BLUE).draw(GL_POINTS,
  // VBO);
}
void Renderer::renderSidebar(const float dt) {
  for (auto &[name, shape, rotation] : world->objects) {
    ImGui::SeparatorText(name);

    ImGui::Checkbox(fmt::format("rotate##{}", name).c_str(), &rotation.rotate);
    ImGui::SameLine();
    if (ImGui::Button(fmt::format("reset##{}", name).c_str())) {
      rotation.mat = {1.0};
    }
  }
}