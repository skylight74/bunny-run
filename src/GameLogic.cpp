#include "../include/GameLogic.h"

GameLogic *GameLogic::instance = nullptr;
using namespace std;
GameLogic::GameLogic(int width, int height, GLFWwindow *window) {
  this->width = width;
  this->height = height;
  this->window = window;
  this->instance = this;
  inVertex_attribute = make_pair(0, "inVertex");
  inNormal_attribute = make_pair(1, "inNormal");
}
GameLogic::~GameLogic() {
  // Delete all models
  for (Model *model : models) {
    delete model;
  }

  // Delete all shader programs
  for (ShaderProgram *shaderProgram : shaderPrograms) {
    delete shaderProgram;
  }
}

void GameLogic::Initialize() {
  // Initialize game-specific properties
  yellowCubeIndex = 2;
  groundDistance = startingPostion;

  speed = startingSpeed;
  speedDelta = startingSpeed;
  rounds = 1;
  score = 0;

  shaderPrograms.push_back(
      new ShaderProgram("shaders/vert0.glsl", "shaders/frag0.glsl",
                        {inVertex_attribute, inNormal_attribute}));

  shaderPrograms.push_back(
      new ShaderProgram("shaders/vert1.glsl", "shaders/frag1.glsl",
                        {inVertex_attribute, inNormal_attribute}));
  shaderPrograms.push_back(new ShaderProgram("shaders/quadVertexShader.glsl",
                                             "shaders/quadFragmentShader.glsl",
                                             {inVertex_attribute}));
  shaderPrograms.push_back(
      new ShaderProgram("shaders/yellowCubeVertexShader.glsl",
                        "shaders/yellowCubeFragmentShader.glsl",
                        {inVertex_attribute, inNormal_attribute}));
  shaderPrograms.push_back(new ShaderProgram("shaders/vert_text.glsl",
                                             "shaders/frag_text.glsl", {}));
  shaderPrograms[0]->useProgram();
  glUniform1f(shaderPrograms[0]->getUniformLocation("intensity"), 1000.0);

  bunny = new Bunny(new vec3(0.f, bunnyJumpPlane, -9.5f), 280,
                    new Model("assets/models/bunny.obj"));
  // the next three models are the same cube so we can use the model and just
  cubes.push_back(new Cube(new vec3(-3.f, -4.f, startingPostion),
                           new Model("assets/models/cube.obj")));
  cubes.push_back(
      new Cube(new vec3(3.f, -4.f, startingPostion), cubes[0]->model));
  cubes.push_back(
      new Cube(new vec3(0.f, -4.f, startingPostion), cubes[0]->model));
  cubes[yellowCubeIndex]->isYellow = true;
  // have pointers to it
  // models.push_back(models[1]);
  // models.push_back(models[1]);
  ground.push_back(new Model("assets/models/quad.obj"));
  ground.push_back(ground[0]);
  ground.push_back(ground[0]);
  // TODO enable those before submission
  textRender = new TextRender(width, height, *shaderPrograms[4]);
  textRender->Load("assets/fonts/LiberationSerif-Italic.ttf", 48);
  // T1xture("../assets/textures/sky.jpg", 0);
}
// this function would take avextor array of models and draw them according to
// the shader program vector array
void GameLogic::Render() {

  glClearColor(0.5, 0.5, 0.5, 1);
  glClearDepth(1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // glLoadIdentity();
  // glTranslatef(-2, 0, -10);
  // glRotatef(angle, 0, 1, 0);
  bunny->model->draw(*shaderPrograms[0], bunny->angle, bunny->axis,
                     *bunny->position, vec3(0.5f, 0.5f, 0.5f), camera);
  // glLoadIdentity();
  // glTranslatef(2, 0, -10);
  // glRotatef(-angle, 0, 1, 0);
  for (int i = 0; i < 3; i++) {
    if (i == yellowCubeIndex) {
      if (bunny->position->x < cubes[i]->position->x + 0.5f &&
          bunny->position->x > cubes[i]->position->x - 0.5f &&
          cubes[i]->position->z > bunny->position->z - 0.7f) {
        // Don't render the yellow cube if the bunny is on it
      } else {
        cubes[i]->model->draw(*shaderPrograms[3], cubeAngle, Y,
                              *cubes[i]->position, vec3(0.55f, 1.f, 1.f),
                              camera);
      }
    } else {
      cubes[i]->model->draw(*shaderPrograms[1], cubeAngle, Y,
                            *cubes[i]->position, vec3(0.55f, 1.f, 1.f), camera);
    }
  }
  shaderPrograms[2]->setUniform("groundMovement", groundDistance);
  ground[0]->draw(*shaderPrograms[2], 90, X, vec3(0.f, 5.f, groundDistance),
                  vec3(4.f, -startingPostion, 1.f), camera);
  ground[1]->draw(*shaderPrograms[2], 90, X,
                  vec3(0.f, 5.f, groundDistance - startingPostion),
                  vec3(4.f, -startingPostion, 5.f), camera);
  ground[2]->draw(*shaderPrograms[2], 90, X,
                  vec3(0.f, 5.f, groundDistance + startingPostion),
                  vec3(4.f, -startingPostion, 5.f), camera);
  assert(glGetError() == GL_NO_ERROR);
  if (bunny->isDead) {
    textRender->RenderText("Game Over! Score: " + to_string(score), 25.0f,
                           400.0f, 1.0f, glm::vec3(0.8, 0.2f, 0.2f));
  } else {
    textRender->RenderText("Score: " + to_string(score), 25.0f, 440.0f, 1.0f,
                           glm::vec3(0.5, 0.8f, 0.2f));
  }

  assert(glGetError() == GL_NO_ERROR);
}

void GameLogic::Update() {
  score += (int)(glfwGetTime() * rounds);
  updateBunnyPosition();
  for (auto &cube : cubes) {
    if (!cube->isYellow) {
      if (cube->position->z < bunny->position->z + 0.7f &&
          cube->position->z > bunny->position->z - 0.7f) {
        if (bunny->position->x < cube->position->x + 0.5f &&
            bunny->position->x > cube->position->x - 0.5f) {
          bunny->isDead = true;
        }
      }
    }
  }
  if (cubes[0]->position->z > -3.4f) {
    cubes[0]->position->z = startingPostion;
    cubes[1]->position->z = startingPostion;
    cubes[2]->position->z = startingPostion;
    groundDistance = startingPostion;
    cubes[yellowCubeIndex]->isYellow = false;
    yellowCubeIndex = (rand() % 3);
    cubes[yellowCubeIndex]->isYellow = true;
    rounds++;
    // speedDelta would decrease by a factor such that the postion updating
    // wouln't exceed 0.5f so that the bunny would have enough time to jump
    // between cube so a logaritmic function would be used
    speed += startingSpeed * log(rounds + 1.0f) / log(pow(1000, rounds));
    if (speed > 0.8f) {
      speed = 0.8f;
    }
  }
  if (speedDelta < speed) {
    speedDelta += (speed - speedDelta) / 100;
  }
  for (auto &cube : cubes) {
    cube->position->z += speedDelta;
  }
  groundDistance += speedDelta;
  // bunny moves up and down on the y axis in a sin wave
}

void GameLogic::updateBunnyPosition() {
  // if bunny is took the yellow cube then rotate it 45 degrees around Y
  if (!bunny->isDead) {
    // if (true) {
    bunny->position->y =
        0.5 * sin(glfwGetTime() * 20 * speedDelta) + bunnyJumpPlane;
    if (bunny->position->x < cubes[yellowCubeIndex]->position->x + 0.5f &&
        bunny->position->x > cubes[yellowCubeIndex]->position->x - 0.5f) {
      if (bunny->position->z < cubes[yellowCubeIndex]->position->z + 0.7f &&
          bunny->position->z > cubes[yellowCubeIndex]->position->z - 0.7f) {
        bunny->angle += 5;
        bunny->axis = Y;
        return;
      }
    } else {
      if (bunny->angle % 360 != 280)
        bunny->angle += 5;
      return;
    }
    if (bunny->angle % 360 != 280)
      bunny->angle += 5;
    bunny->axis = Y;
    // score += (int)(glfwGetTime() * speedDelta * rounds);
    return;
  }
  // bunny is dead stop game and rotate bunny 90 degrees around z axis
  bunny->angle = 80;
  bunny->axis = Z;
  bunny->position->y = bunnyJumpPlane;
  rounds = 0;
  speedDelta = speed = 0;
}

void GameLogic::HandleInput(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
  if (instance) {
    instance->handleKeyPress(key, action);
  }
}

void GameLogic::handleKeyPress(int key, int action) {

  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case GLFW_KEY_A:
      if (bunny->position->x > -3.f && !bunny->isDead) {
        bunny->position->x -= 0.5f;
      }
      break;
    case GLFW_KEY_D:
      if (bunny->position->x < 3.f && !bunny->isDead) {
        bunny->position->x += 0.5f;
      }
      break;
    case GLFW_KEY_R:
      // std::cout << "reset" << std::endl;
      // pressing R would reset the game
      resetGame();
      break;
    }
  } else if (action == GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_A:
      // std::cout << "held left" << std::endl;
      if (bunny->position->x > -3.f && !bunny->isDead) {
        bunny->position->x -= 0.5f;
      }
      break;
    case GLFW_KEY_D:
      // std::cout << "held right" << std::endl;
      if (bunny->position->x < 3.f && !bunny->isDead) {
        bunny->position->x += 0.5f;
      }
      break;
    case GLFW_KEY_R:
      // std::cout << "held reset" << std::endl;
      // pressing R would reset the game
      resetGame();
      break;
    }
  }
}
void GameLogic::resetGame() {
  bunny->position->x = 0.f;
  bunny->position->y = bunnyJumpPlane;
  bunny->position->z = -9.5f;
  bunny->angle = 280;
  bunny->axis = Y;
  bunny->isDead = false;
  cubes[0]->position->z = startingPostion;
  cubes[1]->position->z = startingPostion;
  cubes[2]->position->z = startingPostion;
  groundDistance = startingPostion;
  cubes[yellowCubeIndex]->isYellow = false;
  yellowCubeIndex = (rand() % 3);
  cubes[yellowCubeIndex]->isYellow = true;
  rounds = 1;
  score = 0;
  speed = startingSpeed;
  speedDelta = startingSpeed;
}
