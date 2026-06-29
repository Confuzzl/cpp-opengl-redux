#include "gl/glsl_object.h"

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

namespace GL {
Shader::Shader(const GLenum type, const char *const name)
    : ID{glCreateShader(type)}, type{type}, name{name} {
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
Shader::Shader(Shader &&o) noexcept : ID{o.ID}, type{o.type}, name{o.name} {
  o.ID = 0;
}
Shader &Shader::operator=(Shader &&o) noexcept {
  ID = o.ID;
  type = o.type;
  name = o.name;
  o.ID = 0;
  return *this;
}

Program::Program(const std::initializer_list<Shader> shaders)
    : ID{glCreateProgram()} {
  if (!ID) // TODO
    (void)0;
  fmt::println("Program created: {}", ID);
  for (auto &shader : shaders) {
    glAttachShader(ID, shader.ID);
  }
  glLinkProgram(ID);
  GLint success = 0;
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    GLint size = 0;
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &size);
    std::string log;
    log.resize(size);
    glGetProgramInfoLog(ID, size, &size, &log[0]);
    throw std::runtime_error{fmt::format("PROGRAM LINK ERROR {}\n{}", ID, log)};
  }
  // for (auto &shader : shaders) {
  //   glDeleteShader(shader.ID);
  // }
}
Program ::~Program() { glDeleteProgram(ID); }
Program::Program(Program &&o) noexcept : ID{o.ID} { o.ID = 0; };
Program &Program::operator=(Program &&o) noexcept {
  ID = o.ID;
  o.ID = 0;
  return *this;
}
} // namespace GL