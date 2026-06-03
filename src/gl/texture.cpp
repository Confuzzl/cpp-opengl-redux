#include "gl/texture.h"

#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#include <fmt/format.h>
#include <stb_image.h>

#include <stdexcept>
#include <string>

namespace GL {
Texture::Texture(const std::string_view name, const GLenum filter,
                 const GLenum wrap) {
  glCreateTextures(GL_TEXTURE_2D, 1, &ID);
  glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, filter);
  glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, filter);
  glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrap);
  glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrap);

  const std::string path = SOURCE_DIR "/assets/textures/" + std::string{name};

  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  unsigned char *const data =
      stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (!data) {
    stbi_image_free(data);
    throw std::runtime_error{fmt::format("TEXTURE {} FAILED TO LOAD", path)};
  }
  size = {width, height};

  print_info("texture {} ({}) loaded: {} x {}, {} channels", name, ID, width,
             height, channels);
  glTextureStorage2D(ID, 1, GL_RGBA8, width, height);
  glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                      data);
  glGenerateTextureMipmap(ID);
  stbi_image_free(data);
}
Texture::~Texture() { glDeleteTextures(1, &ID); }
Texture::Texture(Texture &&o) noexcept : ID{o.ID}, size{o.size} { o.ID = 0; }
Texture &Texture::operator=(Texture &&o) noexcept {
  ID = o.ID;
  o.ID = 0;
  size = o.size;
  return *this;
}
} // namespace GL