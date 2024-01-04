#include "include/GameLogic.h"

using namespace std;
int gWidth = 640, gHeight = 480;
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

  GameLogic gameLogic(gWidth, gHeight, window);

  glfwSetKeyCallback(window, GameLogic::HandleInput);
  glfwSetWindowSizeCallback(window, reshape);
  gameLogic.Initialize();
  reshape(window, gWidth, gHeight); // need to call this once ourselves

  while (!glfwWindowShouldClose(window)) {
    gameLogic.Update();
    gameLogic.Render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
