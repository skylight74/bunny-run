#ifndef TEXTRENDER_H
#define TEXTRENDER_H

#include "ShaderProgram.h"
#include <GL/glew.h>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

#include FT_FREETYPE_H
// Structure to hold information about each character
struct Character {
  GLuint TextureID;   // Texture ID holding the glyph
  glm::ivec2 Size;    // Size of the glyph
  glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
  GLuint Advance;     // Offset to advance to next glyph
};

class TextRender {
public:
  TextRender();
  TextRender(GLuint width, GLuint height, ShaderProgram &shaderProgram);
  void Load(std::string font, GLuint fontSize);
  void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
                  glm::vec3 color);

private:
  GLuint VAO, VBO;
  std::map<GLchar, Character> Characters;
  GLuint programID; // Shader program ID for text rendering

  void initRenderData();
};

#endif // TEXTRENDER_H
