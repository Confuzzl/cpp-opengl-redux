#pragma once

#include "glad/gl.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "gl_util.h"
#include "util.h"

namespace GL::runtime {
struct Shader : IDHolder {
  Shader(const std::string_view name, const GLenum type);
  ~Shader();
  DEFAULT_MOVE_ONLY_TYPE(Shader);
};
template <GLenum type> struct TypedShader : Shader {
  TypedShader(const std::string_view name) : Shader(name, type) {}
};
using VertexShader = TypedShader<GL_VERTEX_SHADER>;
using TessControlShader = TypedShader<GL_TESS_CONTROL_SHADER>;
using TessEvaluationShader = TypedShader<GL_TESS_EVALUATION_SHADER>;
using GeometryShader = TypedShader<GL_GEOMETRY_SHADER>;
using FragmentShader = TypedShader<GL_FRAGMENT_SHADER>;

struct ShaderSources {
  std::string vert;
  struct {
    GLint inputPatchSize; // https://stackoverflow.com/questions/42484935
    std::string control;
    std::string evaluation;
  } tess;
  std::string geom;
  std::string frag;
};
struct Shaders {
  VertexShader vert;

  GLint inputPatchSize;
  std::optional<TessControlShader> tcs = std::nullopt;
  std::optional<TessEvaluationShader> tes = std::nullopt;

  std::optional<GeometryShader> geom = std::nullopt;

  FragmentShader frag;
  // Shader vert;
  // std::optional<Shader> tcs = std::nullopt;
  // std::optional<Shader> tes = std::nullopt;
  // std::optional<Shader> geom = std::nullopt;
  // Shader frag;
};

struct Program : IDHolder {
  Shaders shaders;

  struct UniformData {
    GLint location;
    GLenum type;
  };
  std::map<std::string, UniformData, std::less<>> uniforms{};
  bool ignoreMissingUniforms = false;

  Program(const ShaderSources &sources);
  ~Program();
  DEFAULT_MOVE_ONLY_TYPE(Program);

  // void draw(GL::VBO<VertexType> &vbo, const GL::EBO &ebo,
  //           const bool reset = true);

private:
  void attachShaders() const;
  void linkProgram() const;
  void setupUniforms();

  template <auto func, typename T, typename... Args>
  void setUniformImpl(const std::string_view name, Args &&...args) {
    static constexpr auto TYPE_MACRO = uniformTypeMacro<T>;

    const auto it = uniforms.find(name);
    if (it != uniforms.end()) {
      const auto [loc, type] = it->second;
      if (type != TYPE_MACRO) {
        throw std::runtime_error{fmt::format(
            "{} | uniform {} expected type {} but got {}", ID, name,
            uniformTypeToString(type), uniformTypeToString(TYPE_MACRO))};
      }
      (*func)(loc, args...);
    } else if (!ignoreMissingUniforms) {
      throw std::runtime_error{
          fmt::format("{} | {} was not a valid uniform name", ID, name)};
    }
  }

  static constexpr decltype(&glUniform1fv) UNIFORM_VECF_FPS[]{
      &glUniform1fv, &glUniform2fv, &glUniform3fv, &glUniform4fv};
  static constexpr decltype(&glUniform1iv) UNIFORM_VECI_FPS[]{
      &glUniform1iv, &glUniform2iv, &glUniform3iv, &glUniform4iv};
  static constexpr decltype(&glUniform1uiv) UNIFORM_VECU_FPS[]{
      &glUniform1uiv, &glUniform2uiv, &glUniform3uiv, &glUniform4uiv};

public:
  void setUniform(const std::string_view name, const float value) {
    setUniformImpl<&glUniform1f, float>(name, value);
  }
  void setUniform(const std::string_view name, const int value) {
    setUniformImpl<&glUniform1i, int>(name, value);
  }
  void setUniform(const std::string_view name, const unsigned int value) {
    setUniformImpl<&glUniform1ui, unsigned int>(name, value);
  }

  template <glm::length_t L>
  void setUniform(const std::string_view name, const glm::vec<L, float> value) {
    setUniformImpl<UNIFORM_VECF_FPS[L], glm::vec<L, float>>(
        name, L, glm::value_ptr(value));
  }
  template <glm::length_t L>
  void setUniform(const std::string_view name, const glm::vec<L, int> value) {
    setUniformImpl<UNIFORM_VECI_FPS[L], glm::vec<L, int>>(
        name, L, glm::value_ptr(value));
  }
  template <glm::length_t L>
  void setUniform(const std::string_view name,
                  const glm::vec<L, unsigned int> value) {
    setUniformImpl<UNIFORM_VECU_FPS[L], glm::vec<L, unsigned int>>(
        name, L, glm::value_ptr(value));
  }

  void setUniform(const std::string_view name, const glm::mat4 &value) {
    setUniformImpl<&glUniformMatrix4fv, glm::mat4>(name, 1, false,
                                                   glm::value_ptr(value));
  }
};
} // namespace GL::runtime