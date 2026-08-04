#include "gl/texture.h"

#include "gl_util.h"
#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#include <fmt/format.h>
// #include <glm/ext/matrix_common.hpp>
#include <stb_image.h>

#include <stdexcept>
#include <string>

static GLuint createTexture() {
  GLuint out;
  glCreateTextures(GL_TEXTURE_2D, 1, &out);
  return out;
}

namespace GL {
// void Texture::STBIDeleter::operator()(unsigned char *const data) {
//   stbi_image_free(data);
// }

Texture::Texture(const std::string_view name, const GLenum filter,
                 const GLenum wrap)
    : IDHolder{createTexture()} {
  glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, filter);
  glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, filter);
  glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrap);
  glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrap);

  const std::string path = SOURCE_DIR "/assets/textures/" + std::string{name};

  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  const auto data =
      stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (!data)
    throw std::runtime_error{fmt::format("TEXTURE {} FAILED TO LOAD", path)};

  size = {width, height};

  print_info("texture {} ({}) loaded: {}x{}, {} channels", name, ID, width,
             height, channels);
  glTextureStorage2D(ID, 1, GL_RGBA8, width, height);
  glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                      data);
  glGenerateTextureMipmap(ID);
}
Texture::~Texture() { glDeleteTextures(1, &ID); }

// Color Texture::sample(const glm::vec2 uv) const {
//   const auto [u, v] = uv;
//   if (u < 0 || u >= size.x || v < 0 || v >= size.y) {
//     throw std::runtime_error{
//         fmt::format("trying to sample texture {} of size {}x{} with ({},{})",
//                     ID, size.x, size.y, u, v)};
//   }

//   const glm::uvec2 bl = uv, br{bl.x + 1, bl.y}, tr{bl.x + 1, bl.y + 1},
//                    tl{bl.x, bl.y + 1};
//   const auto [dx, dy] = uv - glm::vec2{bl};

//   const glm::vec4 a = at(bl), b = at(br), c = at(tr), d = at(tl);
//   const auto i = glm::mix(a, b, dx), j = glm::mix(d, c, dx);
//   const auto k = glm::mix(i, j, dy);

//   return WHITE;
// }
} // namespace GL