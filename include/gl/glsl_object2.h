#pragma once

#include "glad/gl.h"

#include "color.h"
#include "gl/component/fragment.h"
#include "gl/component/geometry.h"
#include "gl/component/tessellation.h"
#include "gl/component/vertex.h"
#include "gl/gl_object.h"
#include "gl/texture.h"
#include "gl/vao.h"
#include "gl_util.h"
#include "util.h"

#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace GL::test {
struct Shader : IDHolder {
  GLenum type;
  const char *name;

  Shader(const GLenum type, const char *const name);
  ~Shader();
  DEFAULT_MOVE_ONLY_TYPE(Shader);
};

struct Program : IDHolder {
  struct UniformData {
    GLint location;
    GLenum type;
  };
  std::map<std::string, UniformData, std::less<>> uniforms{};
  std::map<std::string, GLuint, std::less<>> samplers{};
  bool ignoreMissingUniforms = false;

  template <std::size_t N>
  Program(const Shader (&shaders)[N]) : IDHolder(glCreateProgram()) {
    if (!ID)
      throw std::runtime_error{"FAILED TO CREATE PROGRAM"};

    fmt::println("Program created: {}", ID);

    for (const auto &shader : shaders) {
      glAttachShader(ID, shader.ID);
    }

    linkProgram();
    setupUniforms();
  }

  ~Program();
  DEFAULT_MOVE_ONLY_TYPE(Program);

private:
  void linkProgram() const;
  void setupUniforms();
};
} // namespace GL::test

namespace shaders::test {
namespace detail {
template <typename Derived> struct UniformProgram : ::GL::test::Program {
  using ::GL::test::Program::Program;

private:
  template <auto func, typename T, typename... Args>
  Derived &setUniformImpl(const std::string_view name, Args &&...args) {
    static constexpr auto TYPE_MACRO = GL::uniformTypeMacro<T>;

    const auto it = uniforms.find(name);
    if (it != uniforms.end()) {
      const auto [loc, type] = it->second;
      if (type != TYPE_MACRO) {
        throw std::runtime_error{
            fmt::format("{} | uniform {} expected type {} but got {}", ID, name,
                        GL::uniformTypeToString(type),
                        GL::uniformTypeToString(TYPE_MACRO))};
      }
      (*func)(ID, loc, args...);
    } else if (!ignoreMissingUniforms) {
      throw std::runtime_error{
          fmt::format("{} | {} was not a valid uniform name", ID, name)};
    }
    return static_cast<Derived &>(*this);
  }

  static constexpr decltype(&glProgramUniform1fv) UNIFORM_VECF_FPS[]{
      &glProgramUniform1fv,
      &glProgramUniform2fv,
      &glProgramUniform3fv,
      &glProgramUniform4fv,
  };
  static constexpr decltype(&glProgramUniform1iv) UNIFORM_VECI_FPS[]{
      &glProgramUniform1iv,
      &glProgramUniform2iv,
      &glProgramUniform3iv,
      &glProgramUniform4iv,
  };
  static constexpr decltype(&glProgramUniform1uiv) UNIFORM_VECU_FPS[]{
      &glProgramUniform1uiv,
      &glProgramUniform2uiv,
      &glProgramUniform3uiv,
      &glProgramUniform4uiv,
  };

public:
  auto &setSampler(const std::string_view name, const GL::Texture &tex) {
    const auto it = samplers.find(name);
    if (it == samplers.end()) {
      throw std::runtime_error{
          fmt::format("{} | {} was not a valid sampler name", ID, name)};
    }
    glBindTextureUnit(it->second, tex.ID);
    return static_cast<Derived &>(*this);
  }

  auto &setUniform(const std::string_view name, const float value) {
    return setUniformImpl<&glUniform1f, float>(name, value);
  }
  auto &setUniform(const std::string_view name, const int value) {
    return setUniformImpl<&glUniform1i, int>(name, value);
  }
  auto &setUniform(const std::string_view name, const unsigned int value) {
    return setUniformImpl<&glUniform1ui, unsigned int>(name, value);
  }

  template <glm::length_t L>
  auto &setUniform(const std::string_view name,
                   const glm::vec<L, float> value) {
    return setUniformImpl<UNIFORM_VECF_FPS[L - 1], glm::vec<L, float>>(
        name, 1, glm::value_ptr(value));
  }
  template <glm::length_t L>
  auto &setUniform(const std::string_view name, const glm::vec<L, int> value) {
    return setUniformImpl<UNIFORM_VECI_FPS[L - 1], glm::vec<L, int>>(
        name, 1, glm::value_ptr(value));
  }
  template <glm::length_t L>
  auto &setUniform(const std::string_view name,
                   const glm::vec<L, unsigned int> value) {
    return setUniformImpl<UNIFORM_VECU_FPS[L - 1], glm::vec<L, unsigned int>>(
        name, 1, glm::value_ptr(value));
  }

  auto &setUniform(const std::string_view name, const glm::mat4 &value) {
    return setUniformImpl<&glProgramUniformMatrix4fv, glm::mat4>(
        name, 1, false, glm::value_ptr(value));
  }
};

template <typename Derived, typename V, typename F>
struct BaseProgram : UniformProgram<Derived> {
  using VertexType = typename V::LayoutType;

protected:
  GL::VertexArrayObject<VertexType> vao;

  using UniformProgram<Derived>::UniformProgram;

  void bind(const GL::VBO<VertexType> &vbo) const {
    glUseProgram(::GL::test::Program::ID);
    glBindVertexArray(vao.ID);
    glVertexArrayVertexBuffer(vao.ID, 0, vbo.ID, 0,
                              GL::VBO<VertexType>::STRIDE);
  }
  void bind(const GL::VBO<VertexType> &vbo, const GL::EBO &ebo) const {
    bind(vbo);
    glVertexArrayElementBuffer(vao.ID, ebo.ID);
  }

  template <bool patch, GLint size, typename... BOs>
  void genericDraw(const bool reset, const GLenum primitive,
                   BOs &...bos) const {
    bind(bos...);
    GL::VBO<VertexType> &vbo = std::get<0>(std::tie(bos...));

    if constexpr (patch)
      glPatchParameteri(GL_PATCH_VERTICES, size);
    if constexpr (sizeof...(BOs) == 1) {
      glDrawArrays(primitive, 0, vbo.count);
    } else {
      const GL::EBO &ebo = std::get<1>(std::tie(bos...));
      glDrawElements(primitive, ebo.count, ebo.type,
                     reinterpret_cast<const void *>(0));
    }
    if (reset)
      vbo.reset();
  }
};
template <typename V, typename F>
struct NoTesselationDraw : BaseProgram<NoTesselationDraw<V, F>, V, F> {
  using Base = BaseProgram<NoTesselationDraw<V, F>, V, F>;
  using Base::BaseProgram;
  using typename Base::VertexType;

  void draw(const GLenum primitive, GL::VBO<VertexType> &vbo,
            const bool reset = true) const {
    Base::genericDraw<false, 0>(reset, primitive, vbo);
  }
  void draw(const GLenum primitive, GL::VBO<VertexType> &vbo,
            const GL::EBO &ebo, const bool reset = true) const {
    Base::genericDraw<false, 0>(reset, primitive, vbo, ebo);
  }
};
template <typename V, typename T, typename F>
struct YesTesselationDraw : BaseProgram<YesTesselationDraw<V, T, F>, V, F> {
  using Base = BaseProgram<YesTesselationDraw<V, T, F>, V, F>;
  using Base::BaseProgram;
  using typename Base::VertexType;

  static constexpr auto PATCH_SIZE = T::PATCH_SIZE;

  void draw(GL::VBO<VertexType> &vbo, const bool reset = true) const {
    Base::genericDraw<true, PATCH_SIZE>(reset, GL_PATCHES, vbo);
  }
  void draw(GL::VBO<VertexType> &vbo, const GL::EBO &ebo,
            const bool reset = true) const {
    Base::genericDraw<true, PATCH_SIZE>(reset, GL_PATCHES, vbo, ebo);
  }
};
template <typename V, typename G, typename T, typename F>
struct GenericProgram : YesTesselationDraw<V, T, F> {
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
  GenericProgram()
      : YesTesselationDraw<V, T, F>({
            {GL_VERTEX_SHADER, V::name},
            {GL_FRAGMENT_SHADER, F::name},
            {GL_TESS_CONTROL_SHADER, T::controlName},
            {GL_TESS_EVALUATION_SHADER, T::evalName},
        }) {}
};
template <typename V, typename G, typename F>
struct GenericProgram<V, G, void, F> : NoTesselationDraw<V, F> {
  GenericProgram()
      : NoTesselationDraw<V, F>({
            {GL_VERTEX_SHADER, V::name},
            {GL_FRAGMENT_SHADER, F::name},
            {GL_GEOMETRY_SHADER, G::name},
        }) {}
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

using Basic = detail::GenericProgram<vert::Basic, void, void, frag::Basic>;
using Texcol = detail::GenericProgram<vert::Tex, void, void, frag::Texcol>;
using Texcol2 = detail::GenericProgram<vert::Tex2, void, void, frag::Texcol>;
using Flat = detail::GenericProgram<vert::Normal, void, void, frag::Flat>;
using Normal = detail::GenericProgram<vert::Normal, void, void, frag::Normal>;
using Phong = detail::GenericProgram<vert::Normal, void, void, frag::Phong>;
using Phong2 = detail::GenericProgram<vert::Texnorm, void, void, frag::Phong2>;
using Sphere =
    detail::GenericProgram<vert::Sphere, void, tess::Sphere, frag::Phong>;
using Cylinder =
    detail::GenericProgram<vert::Cylinder, void, tess::Cylinder, frag::Phong>;
using Cone = detail::GenericProgram<vert::Cone, void, tess::Cone, frag::Phong>;
using Torus =
    detail::GenericProgram<vert::Torus, void, tess::Torus, frag::Phong>;
using Superquadric = detail::GenericProgram<vert::Superquadric, void,
                                            tess::Superquadric, frag::Phong>;
using NormalMap =
    detail::GenericProgram<vert::NormalMap, void, void, frag::NormalMap>;
using Align = detail::GenericProgram<vert::Align, void, void, frag::Basic>;
} // namespace shaders::test