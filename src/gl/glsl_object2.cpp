#include "gl/glsl_object2.h"

#include <fmt/base.h>
#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "util.h"

static std::string sourceToString(const std::string &name) {
  std::ifstream in{name};
  if (!in.is_open()) {
    throw std::runtime_error{
        fmt::format("failed to open {}/{}",
                    std::filesystem::current_path().string(), name)};
  }
  return {std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
}

namespace GL::test {
Shader::Shader(const GLenum type, const char *const name)
    : IDHolder(glCreateShader(type)), type{type}, name{name} {
  const std::string temp =
      sourceToString(fmt::format(SOURCE_DIR "/assets/shaders/{}", name));
  const char *chars = temp.c_str();
  glShaderSource(ID, 1, &chars, nullptr);

  glCompileShader(ID);

  GLint success = 0;
  glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint size = 0;
    glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &size);
    std::string log;
    log.resize(size);
    glGetShaderInfoLog(ID, size, &size, &log[0]);
    throw std::runtime_error{
        fmt::format("COMPILATION ERROR {}\n{}", name, log)};
  }
  fmt::println("Successfully compiled {}:{}", name, ID);
}
Shader::~Shader() {
  // fmt::println("deleting shader {}", ID);
  glDeleteShader(ID);
}

// Program::Program(const std::vector<Shader> &shaders)
//     : IDHolder(glCreateProgram()) {
//   if (!ID)
//     throw std::runtime_error{"FAILED TO CREATE PROGRAM"};
//
//   fmt::println("Program created: {}", ID);
//
//   for (const auto &shader : shaders) {
//     glAttachShader(ID, shader.ID);
//   }
//
//   linkProgram();
//   setupUniforms();
// }
Program ::~Program() { glDeleteProgram(ID); }

void Program::linkProgram() const {
  glLinkProgram(ID);
  GLint success;
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    GLint size = 0;
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &size);
    std::string log;
    log.resize(size);
    glGetProgramInfoLog(ID, size, &size, &log[0]);
    throw std::runtime_error{fmt::format("PROGRAM LINK ERROR {}\n{}", ID, log)};
  }
}

enum struct UniformTypeCategory { SCALAR, SAMPLER, IMAGE, ATOMIC };
static UniformTypeCategory getTypeCategory(const GLenum type) {
  switch (type) {
  case GL_FLOAT:
  case GL_FLOAT_VEC2:
  case GL_FLOAT_VEC3:
  case GL_FLOAT_VEC4:
  case GL_DOUBLE:
  case GL_DOUBLE_VEC2:
  case GL_DOUBLE_VEC3:
  case GL_DOUBLE_VEC4:
  case GL_INT:
  case GL_INT_VEC2:
  case GL_INT_VEC3:
  case GL_INT_VEC4:
  case GL_UNSIGNED_INT:
  case GL_UNSIGNED_INT_VEC2:
  case GL_UNSIGNED_INT_VEC3:
  case GL_UNSIGNED_INT_VEC4:
  case GL_BOOL:
  case GL_BOOL_VEC2:
  case GL_BOOL_VEC3:
  case GL_BOOL_VEC4:
  case GL_FLOAT_MAT2:
  case GL_FLOAT_MAT3:
  case GL_FLOAT_MAT4:
  case GL_FLOAT_MAT2x3:
  case GL_FLOAT_MAT2x4:
  case GL_FLOAT_MAT3x2:
  case GL_FLOAT_MAT3x4:
  case GL_FLOAT_MAT4x2:
  case GL_FLOAT_MAT4x3:
  case GL_DOUBLE_MAT2:
  case GL_DOUBLE_MAT3:
  case GL_DOUBLE_MAT4:
  case GL_DOUBLE_MAT2x3:
  case GL_DOUBLE_MAT2x4:
  case GL_DOUBLE_MAT3x2:
  case GL_DOUBLE_MAT3x4:
  case GL_DOUBLE_MAT4x2:
  case GL_DOUBLE_MAT4x3:
    return UniformTypeCategory::SCALAR;
  case GL_SAMPLER_1D:
  case GL_SAMPLER_2D:
  case GL_SAMPLER_3D:
  case GL_SAMPLER_CUBE:
  case GL_SAMPLER_1D_SHADOW:
  case GL_SAMPLER_2D_SHADOW:
  case GL_SAMPLER_1D_ARRAY:
  case GL_SAMPLER_2D_ARRAY:
  case GL_SAMPLER_1D_ARRAY_SHADOW:
  case GL_SAMPLER_2D_ARRAY_SHADOW:
  case GL_SAMPLER_2D_MULTISAMPLE:
  case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_SAMPLER_CUBE_SHADOW:
  case GL_SAMPLER_BUFFER:
  case GL_SAMPLER_2D_RECT:
  case GL_SAMPLER_2D_RECT_SHADOW:
  case GL_INT_SAMPLER_1D:
  case GL_INT_SAMPLER_2D:
  case GL_INT_SAMPLER_3D:
  case GL_INT_SAMPLER_CUBE:
  case GL_INT_SAMPLER_1D_ARRAY:
  case GL_INT_SAMPLER_2D_ARRAY:
  case GL_INT_SAMPLER_2D_MULTISAMPLE:
  case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_INT_SAMPLER_BUFFER:
  case GL_INT_SAMPLER_2D_RECT:
  case GL_UNSIGNED_INT_SAMPLER_1D:
  case GL_UNSIGNED_INT_SAMPLER_2D:
  case GL_UNSIGNED_INT_SAMPLER_3D:
  case GL_UNSIGNED_INT_SAMPLER_CUBE:
  case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_BUFFER:
  case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    return UniformTypeCategory::SAMPLER;
  case GL_IMAGE_1D:
  case GL_IMAGE_2D:
  case GL_IMAGE_3D:
  case GL_IMAGE_2D_RECT:
  case GL_IMAGE_CUBE:
  case GL_IMAGE_BUFFER:
  case GL_IMAGE_1D_ARRAY:
  case GL_IMAGE_2D_ARRAY:
  case GL_IMAGE_2D_MULTISAMPLE:
  case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
  case GL_INT_IMAGE_1D:
  case GL_INT_IMAGE_2D:
  case GL_INT_IMAGE_3D:
  case GL_INT_IMAGE_2D_RECT:
  case GL_INT_IMAGE_CUBE:
  case GL_INT_IMAGE_BUFFER:
  case GL_INT_IMAGE_1D_ARRAY:
  case GL_INT_IMAGE_2D_ARRAY:
  case GL_INT_IMAGE_2D_MULTISAMPLE:
  case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
  case GL_UNSIGNED_INT_IMAGE_1D:
  case GL_UNSIGNED_INT_IMAGE_2D:
  case GL_UNSIGNED_INT_IMAGE_3D:
  case GL_UNSIGNED_INT_IMAGE_2D_RECT:
  case GL_UNSIGNED_INT_IMAGE_CUBE:
  case GL_UNSIGNED_INT_IMAGE_BUFFER:
  case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
  case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
  case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
  case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    return UniformTypeCategory::IMAGE;
  case GL_UNSIGNED_INT_ATOMIC_COUNTER:
    return UniformTypeCategory::ATOMIC;
  }
  UNREACHABLE;
}

void Program::setupUniforms() {
  // https://stackoverflow.com/a/16302850/31514738

  GLint numUniforms;
  glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numUniforms);

  GLint uniformMaxLength;
  glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

  std::string name;
  name.reserve(uniformMaxLength);

  for (auto i = 0; i < numUniforms; i++) {
    GLsizei length;
    [[maybe_unused]] GLsizei size;
    GLenum type;
    glGetActiveUniform(ID, i, uniformMaxLength, &length, &size, &type,
                       name.data());

    const GLint location = glGetUniformLocation(ID, name.c_str());

    if (location == -1) {
      print_warn("block {}", name.c_str());
      continue; // is block
    }

    uniforms.emplace(std::string(name.c_str(), length),
                     UniformData{.location = location, .type = type});
    if (getTypeCategory(type) == UniformTypeCategory::SAMPLER) {
      GLuint binding;
      glGetUniformuiv(ID, location, &binding);
      samplers.emplace(std::string(name.c_str(), length), binding);
    }
  }

  // for (const auto &[name, data] : uniforms) {
  //   fmt::println("{}: {} {}", name, data.location,
  //                GL::uniformTypeToString(data.type));
  // }
}
} // namespace GL::test
