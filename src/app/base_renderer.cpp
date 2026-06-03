#include "app/base_renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app/app.h"

void BaseRenderer::render(const float dt) {
  renderImpl(dt);
  renderSidebarPrelude(dt);
  renderSidebar(dt);
  renderSidebarPostlude();
}
void BaseRenderer::renderSidebarPrelude(const float dt) const {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize({350, static_cast<float>(app().windowSize.y)});
  ImGui::SetNextWindowBgAlpha(0.5f);

  ImGui::Begin("sidebar", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs);

  ImGui::Text("dt = %.4f", dt);
  ImGui::Text("pos = %.1f %.1f %.1f", world->cam.pos.x, world->cam.pos.y,
              world->cam.pos.z);

  ImGui::Separator();

  ImGui::TextUnformatted("ESC to exit");
  ImGui::TextUnformatted("WASD to move");
  ImGui::TextUnformatted("Arrow keys or mouse to rotate");
  ImGui::TextUnformatted("1-8 keys to switch scenes");
  ImGui::TextUnformatted("F1 for wireframe");
  ImGui::TextUnformatted("F2 for flat shading");
  ImGui::TextUnformatted("F3 for normal shading");
  ImGui::TextUnformatted("F4 for phong shading");
  ImGui::TextUnformatted("TAB to toggle cursor lock");
  ImGui::TextUnformatted("X to show axes");
  ImGui::TextUnformatted("L to move light to position");

  ImGui::Separator();
}
void BaseRenderer::renderSidebarPostlude() const {
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void BaseRenderer::renderImpl(const float dt) const {
  {
    using namespace shaders;
    getUBO<uniforms::CameraBlock>().update(world->cam.matrix());
  }

  static const auto &LIGHT_TEXTURE =
      GL::Texture::shared<"light.png", GL_NEAREST>();
  // static GL::Texture tex{"light.png", GL_NEAREST};

  static constexpr vert_lay::postex light[]{
      vert_lay::postex{{-0.5, -0.5, 0}, {0, 0}},
      vert_lay::postex{{+0.5, -0.5, 0}, {1, 0}},
      vert_lay::postex{{+0.5, +0.5, 0}, {1, 1}},
      vert_lay::postex{{-0.5, +0.5, 0}, {0, 1}},
  };

  static constexpr vert_lay::postex2 LIGHT_VERTICES[]{
      vert_lay::postex2{{-0.5, -0.5, 0},
                        GL::uint_10_11_11_rev::from_uv({0, 0})},
      vert_lay::postex2{{+0.5, -0.5, 0},
                        GL::uint_10_11_11_rev::from_uv({1, 0})},
      vert_lay::postex2{{+0.5, +0.5, 0},
                        GL::uint_10_11_11_rev::from_uv({1, 1})},
      vert_lay::postex2{{-0.5, +0.5, 0},
                        GL::uint_10_11_11_rev::from_uv({0, 1})},
  };
  static constexpr glm::vec3 foo = GL::uint_10_11_11_rev::from_uv({0.6, 1.0});

  static constexpr auto INDICES = {
      0, 1, 2, 0, 2, 3,
  };
  static GL::EBO EBO{INDICES};
  static GL::VBO<vert_lay::postex> LIGHT{light};
  // static GL::VBO<vert_lay::postex2> LIGHT{LIGHT_VERTICES};

  // https://gamedev.stackexchange.com/a/150705
  auto billboard = glm::transpose(world->cam.matrix());
  billboard[0][3] = 0;
  billboard[1][3] = 0;
  billboard[2][3] = 0;
  billboard[3] = glm::vec4{world->light, 1.0};

  // app()
  //     .shaders.texcol2.bindTextureSampler(LIGHT_TEXTURE)
  //     .setModel(billboard)
  //     .setFragColor(world->lightColor)
  //     .draw(GL_TRIANGLES, LIGHT, EBO);
  app()
      .shaders.texcol.bindTextureSampler(LIGHT_TEXTURE)
      .setModel(billboard)
      .setFragColor(world->lightColor)
      .draw(GL_TRIANGLES, LIGHT, EBO);

  static auto AXIS = GL::VBO<>{2};
  static constexpr Color COLORS[3]{RED, GREEN, BLUE};
  if (world->showAxes)
    for (int i = 0; i < 3; i++) {
      glm::vec3 p{};
      p[i] = 10;
      AXIS.write({{}});
      AXIS.write({p});
      app()
          .shaders
          .basic
          //.setCamera(world->cam.matrix())
          .setModel({1.0f})
          .setFragColor(COLORS[i])
          .draw(GL_LINES, AXIS);
    }
}