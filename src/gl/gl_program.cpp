#include "gl/gl_program.h"

#include <fstream>

static std::string sourceToString(const std::string &name) {
  std::ifstream in{name};
  if (!in.is_open()) {
    throw std::runtime_error{fmt::format("failed to open {}", name)};
  }
  return {std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
}

namespace GL::runtime {
Shader::Shader(const std::string_view name, const GLenum type)
    : IDHolder(glCreateShader(type)) {
  const auto source =
      sourceToString(fmt::format(SOURCE_DIR "/assets/shaders/{}", name));
  const char *const chars = source.c_str();
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
        fmt::format("2 COMPILATION ERROR {}\n{}", name, log)};
  }
  fmt::println("2 Successfully compiled {}:{}", name, ID);
}
Shader::~Shader() {
  if (ID) {
    glDeleteShader(ID);
    fmt::println("2 deleting shader");
  }
}

Program::Program(const ShaderSources &sources)
    : IDHolder(glCreateProgram()),
      shaders{.vert{sources.vert},
              .inputPatchSize = sources.tess.inputPatchSize,
              .tcs = sources.tess.control.size()
                         ? std::make_optional<TessControlShader>(
                               sources.tess.control)
                         : std::nullopt,
              .tes = sources.tess.evaluation.size()
                         ? std::make_optional<TessEvaluationShader>(
                               sources.tess.evaluation)
                         : std::nullopt,
              .geom = sources.geom.size()
                          ? std::make_optional<GeometryShader>(sources.geom)
                          : std::nullopt,
              .frag{sources.frag}

      } {
  attachShaders();
  linkProgram();
  setupUniforms();
}
Program::~Program() {
  if (ID)
    glDeleteProgram(ID);
}
void Program::attachShaders() const {
  glAttachShader(ID, shaders.vert.ID);
  if (shaders.tcs)
    glAttachShader(ID, shaders.tcs->ID);
  if (shaders.tes)
    glAttachShader(ID, shaders.tes->ID);
  if (shaders.geom)
    glAttachShader(ID, shaders.geom->ID);
  glAttachShader(ID, shaders.frag.ID);
}
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

    if (location != -1) // block otherwise
      uniforms.emplace(std::string(name.c_str(), length),
                       UniformData{.location = location, .type = type});
  }

  for (const auto &[name, data] : uniforms) {
    fmt::println("{}: {} {}", name, data.location,
                 GL::uniformTypeToString(data.type));
  }
}
} // namespace GL::runtime