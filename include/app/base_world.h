#pragma once

#include "color.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
  static constexpr glm::vec3 UP{0, 1, 0};
  static constexpr glm::vec3 DEFAULT_POS{0, 0, 3};

  glm::vec3 pos = DEFAULT_POS;
  glm::vec3 front, up, right;
  glm::vec3 flatFront;

  float speed = 5;
  float sensitivity = 0.5f;

private:
  float yaw = -90, pitch = 0 /*, roll = 0*/; // in degrees

public:
  Camera() { update(); }

  void update() {
    const float y = glm::radians(yaw), p = glm::radians(pitch);
    front = glm::normalize(glm::vec3{
        std::cos(y) * std::cos(p),
        std::sin(p),
        std::sin(y) * std::cos(p),
    });
    right = glm::normalize(glm::cross(front, UP));
    up = glm::normalize(glm::cross(right, front));

    flatFront = {std::cos(y), 0, std::sin(y)};
  }
  void move(const glm::vec3 dir, const float dt) { pos += dir * speed * dt; }
  void rotate(const float x, const float y) {
    setRotation(yaw + x * sensitivity, pitch + y * sensitivity);
  }
  void setRotation(const float y, const float p) {
    yaw = y;
    pitch = std::max(-89.9f, std::min(p, +89.9f));
    update();
  }

  glm::mat4 matrix() const { return glm::lookAt(pos, pos + front, up); }
};

struct BaseWorld {
  Camera cam;

  Color skyColor{glm::vec4{0.45f, 0.55f, 0.60f, 1.00f}};

  Color lightColor = WHITE;
  glm::vec3 light;

  enum struct RenderState {
    WIREFRAME,
    FLAT,
    NORMAL,
    PHONG
  } renderState = RenderState::FLAT;

  bool showAxes = true;

  BaseWorld(const glm::vec3 light = {2, 0.5, 3});

  virtual void update(const float dt) { (void)dt; }
};