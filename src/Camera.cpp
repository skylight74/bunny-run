#include "../include/Camera.h"

Camera::Camera() {
  // Initialize the camera's position
  position = glm::vec3(0.0f, 10.0f, 10.0f); // Adjust as needed

  // Initialize the camera's direction
  direction = glm::vec3(0.0f, 0.0f, -1.0f); // Looking along -Z axis

  // Initialize FOV, aspect ratio, and clipping planes
  fov = 60.0f;               // Field of view
  aspectRatio = 3.0f / 4.0f; // Default aspect ratio
  nearPlane = 0.1f;          // Near clipping plane
  farPlane = 100.0f;         // Far clipping plane

  // Calculate the initial view and projection matrices
  calculateViewMatrix();
  calculateProjectionMatrix();
}

void Camera::calculateViewMatrix() {
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Up vector
  viewMatrix = glm::lookAt(position, position + direction, up);
}

void Camera::calculateProjectionMatrix() {
  projectionMatrix =
      glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix() const { return viewMatrix; }

glm::mat4 Camera::getProjectionMatrix() const { return projectionMatrix; }

void Camera::updatePosition(const glm::vec3 &positionDelta) {
  // Update the camera's position based on the bunny's position
  position += positionDelta;

  // Update the view matrix
  calculateViewMatrix();
}
glm::vec3 Camera::getPosition() const { return position; }
