#include "include/Model.h"
#include "include/ShaderProgram.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ostream>

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    // Error handling
    std::cerr << "GLFW initialization failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  GLFWwindow *window = glfwCreateWindow(640, 480, "Bunny Run", NULL, NULL);
  if (!window) {
    // Error handling
    std::cerr << "Window creation failed" << std::endl;
    std::cerr << "Terminating GLFW" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    // Error handling
    std::cerr << "GLEW initialization failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set clear color
  // glEnable(GL_DEPTH_TEST); // Temporarily disabled for testing

  // Set viewport
  // glViewport(0, 0, 640, 480);

  // Setup projection matrix (for camera's perspective)
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

  // Setup view matrix (for camera's position and orientation)
  glm::mat4 view = glm::lookAt(
      glm::vec3(0.0f, 0.0f, 3.0f), // Camera is at (0,0,3), in World Space
      glm::vec3(0.0f, 0.0f, 0.0f), // Looks at the origin
      glm::vec3(0.0f, 1.0f,
                0.0f) // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Create and set up the shader program
  ShaderProgram shaderProgram;
  shaderProgram.attachShader(
      "/home/mohamed/ceng477/HW3_24/shaders/vert0-test.glsl", GL_VERTEX_SHADER);
  shaderProgram.attachShader(
      "/home/mohamed/ceng477/HW3_24/shaders/frag0-test.glsl",
      GL_FRAGMENT_SHADER);
  shaderProgram.linkProgram();

  // Load your model
  Model cube("/home/mohamed/ceng477/HW3_24/assets/models/cube.obj");
  glm::mat4 model = glm::mat4(1.0f); // Model matrix
  // After setting up shaders and before the main loop
  GLuint vao, vbo;
  GLfloat vertices[] = {
      -0.5f, -0.5f, 0.0f, // Vertex 1
      0.5f,  -0.5f, 0.0f, // Vertex 2
      0.0f,  0.5f,  0.0f  // Vertex 3
  };

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.useProgram();

    // Set shader uniforms for projection and view matrices
    shaderProgram.setUniform("projection", projection);
    shaderProgram.setUniform("view", view);

    // Draw your model
    // cube.draw();

    // In the main loop, replace bunny.draw() with:
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // OpenGL error checking
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
      std::cerr << "OpenGL error: " << err << std::endl;
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
