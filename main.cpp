#include "include/Model.h"
#include "include/ShaderProgram.h"
#include <GL/gl.h>      // The GL Header File
#include <GL/glew.h>    // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

GLint gIntensityLoc;
float gIntensity = 1000;
int gWidth = 640, gHeight = 480;

// this function would take avextor array of models and draw them according to
// the shader program vector array
void display(std::vector<Model *> &models,
             std::vector<ShaderProgram *> &shaderPrograms) {
  glClearColor(0, 0, 0, 1);
  glClearDepth(1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  static float angle = 0;

  // glLoadIdentity();
  // glTranslatef(-2, 0, -10);
  // glRotatef(angle, 0, 1, 0);
  glm::mat4 T = glm::translate(glm::mat4(1.f), glm::vec3(-2.f, 0.f, -10.f));
  glm::mat4 R =
      glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0, 1, 0));
  glm::mat4 modelMat = T * R;
  glm::mat4 modelMatInv = glm::transpose(glm::inverse(modelMat));
  glm::mat4 perspMat = glm::perspective(glm::radians(45.0f), 1.f, 1.0f, 100.0f);

  shaderPrograms[0]->setUniform("modelingMat", modelMat);
  shaderPrograms[0]->setUniform("modelingMatInvTr", modelMatInv);
  shaderPrograms[0]->setUniform("perspectiveMat", perspMat);

  models[0]->draw(*shaderPrograms[0]);
  // glLoadIdentity();
  // glTranslatef(2, 0, -10);
  // glRotatef(-angle, 0, 1, 0);
  T = glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, -10.f));
  R = glm::rotate(glm::mat4(1.f), glm::radians(-angle), glm::vec3(0, 1, 0));
  modelMat = T * R;
  modelMatInv = glm::transpose(glm::inverse(modelMat));
  //
  shaderPrograms[1]->setUniform("modelingMat", modelMat);
  shaderPrograms[1]->setUniform("modelingMatInvTr", modelMatInv);
  shaderPrograms[1]->setUniform("perspectiveMat", perspMat);

  models[1]->draw(*shaderPrograms[1]);
  assert(glGetError() == GL_NO_ERROR);

  assert(glGetError() == GL_NO_ERROR);

  angle -= 0.5;
}

void reshape(GLFWwindow *window, int w, int h) {
  w = w < 1 ? 1 : w;
  h = h < 1 ? 1 : h;

  gWidth = w;
  gHeight = h;

  glViewport(0, 0, w, h);
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

  glEnable(GL_DEPTH_TEST);

  std::vector<ShaderProgram *> shaderPrograms;
  shaderPrograms.push_back(new ShaderProgram());
  shaderPrograms.push_back(new ShaderProgram());
  shaderPrograms[0]->attachShader("./shaders/vert0.glsl", GL_VERTEX_SHADER);
  shaderPrograms[0]->attachShader("./shaders/frag0.glsl", GL_FRAGMENT_SHADER);
  shaderPrograms[1]->attachShader("./shaders/vert1.glsl", GL_VERTEX_SHADER);
  shaderPrograms[1]->attachShader("./shaders/frag1.glsl", GL_FRAGMENT_SHADER);

  shaderPrograms[0]->bindAttribute(0, "inVertex");
  shaderPrograms[0]->bindAttribute(1, "inNormal");
  shaderPrograms[1]->bindAttribute(0, "inVertex");
  shaderPrograms[1]->bindAttribute(1, "inNormal");

  shaderPrograms[0]->linkProgram();
  shaderPrograms[1]->linkProgram();

  shaderPrograms[0]->useProgram();

  gIntensityLoc = shaderPrograms[0]->getUniformLocation("intensity");
  cout << "gIntensityLoc = " << gIntensityLoc << endl;
  glUniform1f(gIntensityLoc, gIntensity);
  std::vector<Model *> models;
  models.push_back(
      new Model("/home/mohamed/ceng477/HW3_24/assets/models/cube.obj"));
  models.push_back(
      new Model("/home/mohamed/ceng477/HW3_24/assets/models/bunny.obj"));

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
