#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Camera.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "TextRenderer.h"
#include <GLFW/glfw3.h>
#include <vector>

// define a simple cube struct that has the color and position and the model
// related to it. it would save the postition and the model as a pointer.
// the color is not a pointer because it is a simple vec3.
struct Cube {
  bool isYellow;
  vec3 *position;
  Model *model;
  Cube(vec3 *position, Model *model) : position(position), model(model) {
    isYellow = false;
  }
};
// define a struct for the bunny that has the position and the model related to
// it but it would save it as a pointer to the position and angle.
struct Bunny {
  vec3 *position;
  int angle;
  Model *model;
  bool isDead;
  Axis axis;
  Bunny(vec3 *position, float angle, Model *model)
      : position(position), angle(angle), model(model) {
    isDead = false;
    axis = Axis::Y;
  }
};
// #include FT_FREETYPE_H
class GameLogic {
public:
  GameLogic(int width, int height, GLFWwindow *window);
  ~GameLogic();

  void Initialize();
  void Update();
  void Render();

  static void HandleInput(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

private:
  static GameLogic *instance;
  // opengl properties
  GLFWwindow *window;
  int width, height;
  pair<GLuint, std::string> inVertex_attribute;
  pair<GLuint, std::string> inNormal_attribute;
  // Game objects and programs
  std::vector<Model *> models;
  std::vector<Cube *> cubes;
  Bunny *bunny;
  std::vector<Model *> ground;
  std::vector<ShaderProgram *> shaderPrograms;
  Camera camera;
  TextRender *textRender;
  // Game-specific properties
  int score, rounds;
  float speed;
  float speedDelta;
  const float startingSpeed = 0.3f;
  const float startingPostion = -75.f;
  const float bunnyJumpPlane = -4.4f;
  const float cubeAngle = 0;
  // Other properties as needed for game state
  int yellowCubeIndex;
  float groundDistance;
  // Game logic states

  // Private methods for internal functionality
  void updateBunnyPosition();
  void handleKeyPress(int key, int action);
  void resetGame();
  // Other methods as needed
};

#endif // GAMELOGIC_H
