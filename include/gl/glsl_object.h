#pragma once

#define SET_UNIFORM_TEMPLATE(U, T, call)                                       \
  template <>                                                                  \
  inline void setUniform(const Program &p, const GL::Uniform<U> &uniform,      \
                         const T value) {                                      \
    call;                                                                      \
  }
#define SET_SCALAR(type, scalar_type)                                          \
  SET_UNIFORM_TEMPLATE(                                                        \
      type, type,                                                              \
      glProgramUniform1##scalar_type(p.ID, uniform.location, value))
#define SET_VECTOR(type, vector_type)                                          \
  SET_UNIFORM_TEMPLATE(type, type,                                             \
                       glProgramUniform##vector_type##v(                       \
                           p.ID, uniform.location, 1, glm::value_ptr(value)))
#define SET_MATRIX(type, matrix_type)                                          \
  SET_UNIFORM_TEMPLATE(                                                        \
      type, type,                                                              \
      glProgramUniformMatrix##matrix_type##fv(                                 \
          p.ID, uniform.location, 1, GL_FALSE, glm::value_ptr(value)))

#define SET_UNIFORM(func_name, param_t, param_name, shader, def)               \
  auto &set##func_name(const param_t param_name def) {                         \
    setUniform(*this, shader.param_name, param_name);                          \
    return *this;                                                              \
  }
#define SET_UNIFORM_V(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, vertex, def)
#define SET_UNIFORM_F(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, fragment, def)
#define SET_UNIFORM_G(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, geometry, def)
#define SET_UNIFORM_T(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, tessellation, def)

#define BIND_TEXTURE(sampler, sampler_name)                                    \
  auto &bindTexture##sampler(const GL::Texture &texture) {                     \
    glBindTextureUnit(fragment.sampler_name.binding, texture.ID);              \
    return *this;                                                              \
  }

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "color.h"
#include "gl/component/fragment.h"
#include "gl/component/geometry.h"
#include "gl/component/tessellation.h"
#include "gl/component/vertex.h"
#include "gl/gl_object.h"
// #include "gl/uniform.h"
#include "gl/texture.h"
#include "gl/vao.h"
#include "util.h"

#include <initializer_list>

namespace GL {
struct Shader {
  GLuint ID;
  GLenum type;
  const char *name;

  Shader(const GLenum type, const char *const name);
  ~Shader();
  MOVE_ONLY_TYPE(Shader)
};

struct Program {
  GLuint ID;

  Program(const std::initializer_list<Shader> shaders);
  ~Program();
  MOVE_ONLY_TYPE(Program)
};
template <typename U, typename T>
void setUniform(const Program &p, const U &uniform, const T value);
SET_SCALAR(bool, i)
SET_SCALAR(int, i)
SET_SCALAR(unsigned int, ui)
SET_SCALAR(Color, ui)
SET_SCALAR(float, f)
SET_VECTOR(glm::vec2, 2f)
SET_VECTOR(glm::vec3, 3f)
SET_VECTOR(glm::uvec2, 2ui)
SET_VECTOR(glm::uvec3, 3ui)
template <>
inline void setUniform(const Program &p, const GL::Uniform<glm::mat4> &uniform,
                       const glm::mat4 value) {
  glad_glProgramUniformMatrix4fv(p.ID, uniform.location, 1, 0,
                                 glm::value_ptr(value));
}
} // namespace GL

namespace shaders {
namespace detail {
template <typename V, typename F> struct BaseProgram : ::GL::Program {
  using VertexType = typename V::LayoutType;

protected:
  GL::VertexArrayObject<VertexType> vao;
  V vertex;
  F fragment;

  BaseProgram(const std::initializer_list<GL::Shader> shaders)
      : Program{shaders}, vertex{ID}, fragment{ID} {}

  void bind(const GL::VBO<VertexType> &vbo) const {
    glUseProgram(ID);
    glBindVertexArray(vao.ID);
    glVertexArrayVertexBuffer(vao.ID, 0, vbo.ID, 0,
                              GL::VBO<VertexType>::STRIDE);
  }
  void bind(const GL::VBO<VertexType> &vbo, const GL::EBO &ebo) const {
    bind(vbo);
    glVertexArrayElementBuffer(vao.ID, ebo.ID);
  }
};
template <typename V, typename F> struct NoTesselationDraw : BaseProgram<V, F> {
  using Base = BaseProgram<V, F>;
  using Base::BaseProgram;
  using typename Base::VertexType;

  void draw(const GLenum primitive, GL::VBO<VertexType> &vbo,
            const bool reset = true) const {
    Base::bind(vbo);
    glDrawArrays(primitive, 0, vbo.count);
    if (reset)
      vbo.reset();
  }
  void draw(const GLenum primitive, GL::VBO<VertexType> &vbo,
            const GL::EBO &ebo, const bool reset = true) const {
    Base::bind(vbo, ebo);
    glDrawElements(primitive, ebo.count, ebo.type,
                   reinterpret_cast<const void *>(0));
    if (reset)
      vbo.reset();
  }
};
template <typename V, typename T, typename F>
struct YesTesselationDraw : BaseProgram<V, F> {
  using BaseProgram<V, F>::BaseProgram;

  using Base = BaseProgram<V, F>;
  using typename Base::VertexType;

  static constexpr auto PATCH_SIZE = T::PATCH_SIZE;

  void draw(GL::VBO<VertexType> &vbo, const bool reset = true) const {
    Base::bind(vbo);
    glPatchParameteri(GL_PATCH_VERTICES, PATCH_SIZE);
    glDrawArrays(GL_PATCHES, 0, vbo.count);
    if (reset)
      vbo.reset();
  }
  void draw(GL::VBO<VertexType> &vbo, const GL::EBO &ebo,
            const bool reset = true) const {
    Base::bind(vbo, ebo);
    glPatchParameteri(GL_PATCH_VERTICES, PATCH_SIZE);
    glDrawElements(GL_PATCHES, ebo.count, ebo.type,
                   reinterpret_cast<const void *>(0));
    if (reset)
      vbo.reset();
  }
};
template <typename V, typename G, typename T, typename F>
struct GenericProgram : YesTesselationDraw<V, T, F> {
  T tessellation;
  G geometry;

  GenericProgram()
      : YesTesselationDraw<V, T, F>({
            {GL_VERTEX_SHADER, V::name},
            {GL_FRAGMENT_SHADER, F::name},
            {GL_TESS_CONTROL_SHADER, T::controlName},
            {GL_TESS_EVALUATION_SHADER, T::evalName},
            {GL_GEOMETRY_SHADER, G::name},
        }) {}
};
template <typename V, typename F, typename T>
struct GenericProgram<V, void, T, F> : YesTesselationDraw<V, T, F> {
  T tessellation;
  GenericProgram()
      : YesTesselationDraw<V, T, F>({
            {GL_VERTEX_SHADER, V::name},
            {GL_FRAGMENT_SHADER, F::name},
            {GL_TESS_CONTROL_SHADER, T::controlName},
            {GL_TESS_EVALUATION_SHADER, T::evalName},
        }),
        tessellation{::GL::Program::ID} {}
};
template <typename V, typename G, typename F>
struct GenericProgram<V, G, void, F> : NoTesselationDraw<V, F> {
  G geometry;
  GenericProgram()
      : NoTesselationDraw<V, F>({
            {GL_VERTEX_SHADER, V::name},
            {GL_FRAGMENT_SHADER, F::name},
            {GL_GEOMETRY_SHADER, G::name},
        }),
        geometry{::GL::Program::ID} {}
};
template <typename V, typename F>
struct GenericProgram<V, void, void, F> : NoTesselationDraw<V, F> {
  GenericProgram()
      : NoTesselationDraw<V, F>({
            {GL_VERTEX_SHADER, V::name},
            {GL_FRAGMENT_SHADER, F::name},
        }) {}
};
} // namespace detail

struct Basic : detail::GenericProgram<vert::Basic, void, void, frag::Basic> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Texcol : detail::GenericProgram<vert::Tex, void, void, frag::Texcol> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  BIND_TEXTURE(Sampler, sampler);
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Texcol2 : detail::GenericProgram<vert::Tex2, void, void, frag::Texcol> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  BIND_TEXTURE(Sampler, sampler);
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};

struct Flat : detail::GenericProgram<vert::Normal, void, void, frag::Flat> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Normal : detail::GenericProgram<vert::Normal, void, void, frag::Normal> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
};
struct Phong : detail::GenericProgram<vert::Normal, void, void, frag::Phong> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Phong2
    : detail::GenericProgram<vert::Texnorm, void, void, frag::Phong2> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  BIND_TEXTURE(Sampler, sampler);
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
};
struct Sphere
    : detail::GenericProgram<vert::Sphere, void, tess::Sphere, frag::Phong> {
  SET_UNIFORM_T(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Cylinder : detail::GenericProgram<vert::Cylinder, void, tess::Cylinder,
                                         frag::Phong> {
  SET_UNIFORM_T(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Cone
    : detail::GenericProgram<vert::Cone, void, tess::Cone, frag::Phong> {
  SET_UNIFORM_T(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct Torus
    : detail::GenericProgram<vert::Torus, void, tess::Torus, frag::Phong> {
  SET_UNIFORM_T(Resolution, float, resolution, )
  SET_UNIFORM_T(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};

struct Superquadric : detail::GenericProgram<vert::Superquadric, void,
                                             tess::Superquadric, frag::Phong> {
  SET_UNIFORM_T(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(LightColor, Color, light_color, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
struct NormalMap
    : detail::GenericProgram<vert::NormalMap, void, void, frag::NormalMap> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  BIND_TEXTURE(DiffuseMap, diffuse_map);
  BIND_TEXTURE(NormalMap, normal_map);
  SET_UNIFORM_F(Light, glm::vec3, light, )
  SET_UNIFORM_F(CameraPos, glm::vec3, camera_pos, )
};
struct Align : detail::GenericProgram<vert::Align, void, void, frag::Basic> {
  SET_UNIFORM_V(Model, glm::mat4 &, model, )
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
} // namespace shaders

#undef SET_UNIFORM_TEMPLATE
#undef SET_SCALAR
#undef SET_VECTOR_2
#undef SET_VECTOR
#undef SET_MATRIX
#undef SET_UNIFORM
#undef SET_UNIFORM_V
#undef SET_UNIFORM_F
#undef SET_UNIFORM_G
#undef BIND_TEXTURE