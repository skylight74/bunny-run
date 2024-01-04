#define STB_IMAGE_IMPLEMENTATION
#include "../include/Texture.h"
#include "../include/stb_image.h"
#include <iostream>

Texture::Texture(const std::string &filePath) {
  stbi_set_flip_vertically_on_load(1);
  unsigned char *data = loadImage(filePath);
  if (data) {
    glGenTextures(1, &textureID);
    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);

    unbind();
    stbi_image_free(data);
  }
}

unsigned char *Texture::loadImage(const std::string &filePath) {
  unsigned char *data =
      stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
  if (!data) {
    std::cerr << "Failed to load texture: " << filePath << std::endl;
  }
  return data;
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture::~Texture() { glDeleteTextures(1, &textureID); }
