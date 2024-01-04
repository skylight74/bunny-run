#include "../include/TextRenderer.h"

using namespace std;
using namespace glm;

TextRender::TextRender() {
  // Activate corresponding render state
  glUseProgram(programID);
  glUniform3f(glGetUniformLocation(programID, "textColor"), 1.0f, 1.0f, 1.0f);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glUseProgram(0);
}

void TextRender::RenderText(string text, GLfloat x, GLfloat y, GLfloat scale,
                            vec3 color) {
  // Activate corresponding render state
  glUseProgram(programID);
  glUniform3f(glGetUniformLocation(programID, "textColor"), color.x, color.y,
              color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // Iterate through all characters
  std::string::const_iterator c;

  for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
        {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
        {xpos + w, ypos, 1.0, 1.0},

        {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
        {xpos + w, ypos + h, 1.0, 0.0}};
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Now advance cursors for next glyph (note that advance is number of 1/64
    // pixels)
    x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6
                                    // = 64)
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

void TextRender::Load(string font, GLuint fontSize) {
  // First clear the previously loaded Characters
  Characters.clear();
  // Then initialize and load the FreeType library
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) // All functions return a value different than 0
                             // whenever an error occurred
    cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;

  // Load font as face
  FT_Face face;
  if (FT_New_Face(ft, font.c_str(), 0, &face))
    cout << "ERROR::FREETYPE: Failed to load font" << endl;

  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, fontSize);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Then for the first 128 ASCII characters, pre-load/compile their characters
  // and store them
  for (GLubyte c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
      continue;
    }
    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // Note that we set the container to
                                       // GL_CLAMP_TO_EDGE to prevent semi-
                                       // transparent borders. Due to
                                       // interpolation it takes texels from
                                       // next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR); // GL_LINEAR here is said to be better than
                                // GL_NEAREST for text rendering, since it
                                // uses linear interpolation for better
                                // font rendering quality.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Now store character for later use
    Character character = {
        texture, ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        GLuint(face->glyph->advance.x)};
    Characters.insert(pair<GLchar, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  // Configure VAO/VBO for texture quads
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void TextRender::initRenderData() {
  // Configure VAO/VBO for texture quads
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Dynamic draw because we will be updating the VBO every frame
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

TextRender::TextRender(GLuint width, GLuint height, ShaderProgram &shader) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Compile and setup the shader
  programID = shader.getProgramID();
  // Set projection matrix
  mat4 projection = ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                          static_cast<GLfloat>(height));
  shader.useProgram();
  glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE,
                     value_ptr(projection));
  initRenderData();
}
