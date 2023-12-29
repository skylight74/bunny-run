#include "include/Model.h"
#include "include/ShaderProgram.h"
#include <GL/glew.h>    // The GL Header File
#include <GL/gl.h>      // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

GLint gIntensityLoc;
float gIntensity = 1000;
int gWidth = 640, gHeight = 480;

GLuint gVertexAttribBuffer, gTextVBO, gIndexBuffer;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  GLuint TextureID;   // ID handle of the glyph texture
  glm::ivec2 Size;    // Size of glyph
  glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
  GLuint Advance;     // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

void initFonts(int windowWidth, int windowHeight, GLint programID) {
  // Set OpenGL options
  // glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth),
                                    0.0f, static_cast<GLfloat>(windowHeight));
  glUseProgram(programID);
  glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE,
                     glm::value_ptr(projection));

  // FreeType
  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
  }

  // Load font as face
  FT_Face face;
  if (FT_New_Face(ft, "/usr/share/fonts/liberation/LiberationSerif-Italic.ttf",
                  0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
  }

  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, 48);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load first 128 characters of ASCII set
  for (GLubyte c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Now store character for later use
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x};
    Characters.insert(std::pair<GLchar, Character>(c, character));
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  //
  // Configure VBO for texture quads
  //
  glGenBuffers(1, &gTextVBO);
  glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderText(const std::string &text, GLfloat x, GLfloat y, GLfloat scale,
                glm::vec3 color, GLint programID) {
  // Activate corresponding render state
  glUseProgram(programID);
  glUniform3f(glGetUniformLocation(programID, "textColor"), color.x, color.y,
              color.z);
  glActiveTexture(GL_TEXTURE0);

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
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(vertices),
        vertices); // Be sure to use glBufferSubData and not glBufferData

    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Now advance cursors for next glyph (note that advance is number of 1/64
    // pixels)

    x += (ch.Advance >> 6) *
         scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount
                // of 1/64th pixels by 64 to get amount of pixels))
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

// this function would take avextor array of models and draw them according to
// the shader program vector array
void display(std::vector<Model> &models,
             std::vector<ShaderProgram> &shaderPrograms) {
  glClearColor(0, 0, 0, 1);
  glClearDepth(1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  static float angle = 0;

  // glLoadIdentity();
  // glTranslatef(-2, 0, -10);
  // glRotatef(angle, 0, 1, 0);
  shaderPrograms[0].useProgram();
  glm::mat4 T = glm::translate(glm::mat4(1.f), glm::vec3(-2.f, 0.f, -10.f));
  glm::mat4 R =
      glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0, 1, 0));
  glm::mat4 modelMat = T * R;
  glm::mat4 modelMatInv = glm::transpose(glm::inverse(modelMat));
  glm::mat4 perspMat = glm::perspective(glm::radians(45.0f), 1.f, 1.0f, 100.0f);
  shaderPrograms[0].setUniform("modelingMat", modelMat);
  shaderPrograms[0].setUniform("modelingMatInvTr", modelMatInv);
  shaderPrograms[0].setUniform("perspectiveMat", perspMat);

  models[0].draw(shaderPrograms[0]);
  // glLoadIdentity();
  // glTranslatef(2, 0, -10);
  // glRotatef(-angle, 0, 1, 0);
  shaderPrograms[1].useProgram();
  T = glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, -10.f));
  R = glm::rotate(glm::mat4(1.f), glm::radians(-angle), glm::vec3(0, 1, 0));
  modelMat = T * R;
  modelMatInv = glm::transpose(glm::inverse(modelMat));
  //
  shaderPrograms[1].setUniform("modelingMat", modelMat);
  shaderPrograms[1].setUniform("modelingMatInvTr", modelMatInv);
  shaderPrograms[1].setUniform("perspectiveMat", perspMat);
  //
  // // drawModel();
  models[1].draw(shaderPrograms[1]);
  assert(glGetError() == GL_NO_ERROR);

  // renderText("CENG 477 - 2022", 0, 0, 1, glm::vec3(0, 1, 1),
  // shaderProgram3.programID);

  assert(glGetError() == GL_NO_ERROR);

  angle += 0.5;
}

void reshape(GLFWwindow *window, int w, int h) {
  w = w < 1 ? 1 : w;
  h = h < 1 ? 1 : h;

  gWidth = w;
  gHeight = h;

  glViewport(0, 0, w, h);
}

void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods,
              GLint &programID1, GLint &programID2) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    cout << "F pressed" << endl;
    glUseProgram(programID2);
  } else if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    cout << "V pressed" << endl;
    glUseProgram(programID1);
  } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    cout << "D pressed" << endl;
    gIntensity /= 1.5;
    cout << "gIntensity = " << gIntensity << endl;
    glUseProgram(programID1);
    glUniform1f(gIntensityLoc, gIntensity);
  } else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    cout << "B pressed" << endl;
    gIntensity *= 1.5;
    cout << "gIntensity = " << gIntensity << endl;
    glUseProgram(programID1);
    glUniform1f(gIntensityLoc, gIntensity);
  }
}

int main(int argc,
         char **argv) // Create Main Function For Bringing It All Together
{
  GLFWwindow *window;
  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(gWidth, gHeight, "Simple Example", NULL, NULL);

  if (!window) {
    glfwTerminate();
    exit(-1);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Initialize GLEW to setup the OpenGL Function pointers
  if (GLEW_OK != glewInit()) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  char rendererInfo[512] = {0};
  strcpy(rendererInfo, (const char *)glGetString(GL_RENDERER));
  strcat(rendererInfo, " - ");
  strcat(rendererInfo, (const char *)glGetString(GL_VERSION));
  glfwSetWindowTitle(window, rendererInfo);

  Model cube("/home/mohamed/ceng477/HW3_24/assets/models/cube.obj");
  Model bunny("/home/mohamed/ceng477/HW3_24/assets/models/bunny.obj");
  std::vector<Model> models;
  models.push_back(cube);
  models.push_back(bunny);

  glEnable(GL_DEPTH_TEST);
  std::vector<ShaderProgram> shaderPrograms;
  ShaderProgram cubeShaderProgram, bunnyShaderProgram, testShaderProgram;
  cubeShaderProgram.attachShader("./shaders/vert0.glsl", GL_VERTEX_SHADER);
  cubeShaderProgram.attachShader("./shaders/frag0.glsl", GL_FRAGMENT_SHADER);
  bunnyShaderProgram.attachShader("./shaders/vert1.glsl", GL_VERTEX_SHADER);
  bunnyShaderProgram.attachShader("./shaders/frag1.glsl", GL_FRAGMENT_SHADER);
  cubeShaderProgram.linkProgram();
  bunnyShaderProgram.linkProgram();
  // testShaderProgram.attachShader("./shaders/vert_text.glsl",
  // GL_VERTEX_SHADER);
  // testShaderProgram.attachShader("./shaders/frag_text.glsl",
  //                                GL_FRAGMENT_SHADER);
  // testShaderProgram.linkProgram();
  cubeShaderProgram.useProgram();
  shaderPrograms.push_back(cubeShaderProgram);
  shaderPrograms.push_back(bunnyShaderProgram);
  // shaderPrograms.push_back(testShaderProgram);
  gIntensityLoc =
      glGetUniformLocation(cubeShaderProgram.programID, "intensity");
  cout << "gIntensityLoc = " << gIntensityLoc << endl;
  glUniform1f(gIntensityLoc, gIntensity);

  // initFonts(gWidth, gHeight, testShaderProgram.programID);

  // glfwSetKeyCallback(window, keyboard);
  glfwSetWindowSizeCallback(window, reshape);

  reshape(window, gWidth, gHeight); // need to call this once ourselves

  while (!glfwWindowShouldClose(window)) {
    display(models, shaderPrograms);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
