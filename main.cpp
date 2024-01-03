#include "include/Camera.h"
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

#define SPEED .07f
#define STARTING_POSITION -75.f
#define BUNNY_JUMP_PLANE -4.4f

using namespace std;
pair<GLuint, std::string> inVertex_attribute = std::make_pair(0, "inVertex");
pair<GLuint, std::string> inNormal_attribute = std::make_pair(1, "inNormal");
GLint gIntensityLoc;
float gIntensity = 1000;
int gWidth = 640, gHeight = 480;
int rounds = 1;
void reshape(GLFWwindow *window, int w, int h) {
  w = w < 1 ? 1 : w;
  h = h < 1 ? 1 : h;

  gWidth = w;
  gHeight = h;

  glViewport(0, 0, w, h);
}
// this function would take avextor array of models and draw them according to
// the shader program vector array
void display(std::vector<Model *> &models,
             std::vector<ShaderProgram *> &shaderPrograms, Camera &camera) {
  glClearColor(0.5, 0.5, 0.5, 1);
  glClearDepth(1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  static float bunnyAngle = 280;
  static float bunnyYpos = BUNNY_JUMP_PLANE;
  static float cubeAngle = 0;
  static float cuberFardistance = STARTING_POSITION;
  static int yellowcube_index = 2;
  static float groundDistance = STARTING_POSITION;
  // glLoadIdentity();
  // glTranslatef(-2, 0, -10);
  // glRotatef(angle, 0, 1, 0);

  models[0]->draw(*shaderPrograms[0], bunnyAngle, Y,
                  vec3(0.f, bunnyYpos, -9.5f), vec3(0.5f, 0.5f, 0.5f), camera);
  // glLoadIdentity();
  // glTranslatef(2, 0, -10);
  // glRotatef(-angle, 0, 1, 0);
  for (int i = 1; i < 4; i++) {
    if (i == yellowcube_index) {
      if (i == 1)
        models[i]->draw(*shaderPrograms[3], cubeAngle, Y,
                        vec3(-3.f, -4.f, cuberFardistance),
                        vec3(.55f, 1.f, 1.f), camera);
      else
        models[i]->draw(*shaderPrograms[3], cubeAngle, Y,
                        vec3(i % 2 * 3.f, -4.f, cuberFardistance),
                        vec3(.55f, 1.f, 1.f), camera);
    } else {
      if (i == 1)
        models[i]->draw(*shaderPrograms[1], cubeAngle, Y,
                        vec3(-3.f, -4.f, cuberFardistance),
                        vec3(.55f, 1.f, 1.f), camera);
      else
        models[i]->draw(*shaderPrograms[1], cubeAngle, Y,
                        vec3(i % 2 * 3.f, -4.f, cuberFardistance),
                        vec3(.55f, 1.f, 1.f), camera);
    }
  }
  shaderPrograms[2]->setUniform("groundMovement", groundDistance);
  models[4]->draw(*shaderPrograms[2], 90, X, vec3(0.f, 5.f, groundDistance),
                  vec3(3.75f, -STARTING_POSITION, 1.f), camera);
  models[4]->draw(*shaderPrograms[2], 90, X,
                  vec3(0.f, 5.f, groundDistance - STARTING_POSITION),
                  vec3(3.75f, -STARTING_POSITION, 5.f), camera);
  models[4]->draw(*shaderPrograms[2], 90, X,
                  vec3(0.f, 5.f, groundDistance + STARTING_POSITION),
                  vec3(3.75f, -STARTING_POSITION, 5.f), camera);
  assert(glGetError() == GL_NO_ERROR);
  if (cuberFardistance > -5.f) {
    cuberFardistance = STARTING_POSITION;
    groundDistance = STARTING_POSITION;
    yellowcube_index = (rand() % 3) + 1;
    rounds++;
  }
  cuberFardistance += SPEED * rounds;
  groundDistance += SPEED * rounds;
  // bunny moves up and down on the y axis in a sin wave
  bunnyYpos = 0.5 * sin(glfwGetTime() * 25 * SPEED * rounds) + BUNNY_JUMP_PLANE;
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

  window = glfwCreateWindow(gWidth, gHeight, "Bunny Run", NULL, NULL);

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

  Camera camera;
  std::vector<ShaderProgram *> shaderPrograms; // TODO make them unique_ptr

  shaderPrograms.push_back(
      new ShaderProgram("./shaders/vert0.glsl", "./shaders/frag0.glsl",
                        {inVertex_attribute, inNormal_attribute}));

  shaderPrograms.push_back(
      new ShaderProgram("./shaders/vert1.glsl", "./shaders/frag1.glsl",
                        {inVertex_attribute, inNormal_attribute}));
  shaderPrograms.push_back(new ShaderProgram(
      "./shaders/quadVertexShader.glsl", "./shaders/quadFragmentShader.glsl",
      {inVertex_attribute}));
  shaderPrograms.push_back(
      new ShaderProgram("./shaders/yellowCubeVertexShader.glsl",
                        "./shaders/yellowCubeFragmentShader.glsl",
                        {inVertex_attribute, inNormal_attribute}));
  shaderPrograms[0]->useProgram();

  gIntensityLoc = shaderPrograms[0]->getUniformLocation("intensity");
  cout << "gIntensityLoc = " << gIntensityLoc << endl;
  glUniform1f(gIntensityLoc, gIntensity);

  std::vector<Model *> models;
  models.push_back(
      new Model("/home/mohamed/ceng477/HW3_24/assets/models/bunny.obj"));

  models.push_back(
      new Model("/home/mohamed/ceng477/HW3_24/assets/models/cube.obj"));

  // the next three models are the same cube so we can use the model and just
  // have pointers to it
  models.push_back(models[1]);
  models.push_back(models[1]);
  models.push_back(
      new Model("/home/mohamed/ceng477/HW3_24/assets/models/quad.obj"));
  models.push_back(models[4]);
  models.push_back(models[4]);

  glfwSetWindowSizeCallback(window, reshape);

  reshape(window, gWidth, gHeight); // need to call this once ourselves

  while (!glfwWindowShouldClose(window)) {
    display(models, shaderPrograms, camera);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
