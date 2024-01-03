#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera();

  // Update the camera's position based on the bunny's position
  void updatePosition(const glm::vec3 &bunnyPosition);
  glm::vec3 getPosition() const;
  // Get the view matrix for rendering
  glm::mat4 getViewMatrix() const;

  // Get the projection matrix for rendering
  glm::mat4 getProjectionMatrix() const;

private:
  glm::vec3 position;
  glm::vec3 direction;
  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;

  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;

  // Calculate the view matrix based on current camera properties
  void calculateViewMatrix();

  // Calculate the projection matrix based on current camera properties
  void calculateProjectionMatrix();
};

#endif // CAMERA_H
