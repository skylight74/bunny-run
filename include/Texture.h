#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture {
public:
    Texture(const std::string &filePath);
    ~Texture();
    void bind(unsigned int slot = 0) const;
    void unbind() const;

private:
    GLuint textureID;
    int width, height, numChannels;
    unsigned char* loadImage(const std::string &filePath);
};

#endif // TEXTURE_H
