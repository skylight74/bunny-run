#include "../include/Model.h"
#include <cassert>
#include <fstream>
#include <glm/fwd.hpp>
#include <iostream>
#include <sstream>

// Constructor
Model::Model(const std::string &filename) {
  if (!parseObj(filename)) {
    std::cerr << "Model loading failed: " << filename << std::endl;
    throw std::runtime_error("Failed to load model");
  }
  setupBuffers();
}

// Vertex, Texture, Normal, Face struct constructors
// Define these constructors here

// Parse .obj file
bool Model::parseObj(const std::string &filename) {
  std::fstream myfile(filename.c_str(), std::ios::in);

  if (!myfile.is_open()) {
    std::cerr << "Failed to open OBJ file: " << filename << std::endl;
    return false;
  }

  std::string curLine;
  while (getline(myfile, curLine)) {
    std::stringstream str(curLine);
    GLfloat c1, c2, c3;
    int vIndex[3], tIndex[3], nIndex[3];
    std::string tmp;
    char c;

    if (curLine[0] == '#')
      continue; // Skip comments
    if (curLine[0] == 'v') {
      if (curLine[1] == 't') { // Texture coordinates
        str >> tmp >> c1 >> c2;
        textures.push_back(Texture(c1, c2));
      } else if (curLine[1] == 'n') { // Normals
        str >> tmp >> c1 >> c2 >> c3;
        normals.push_back(Normal(c1, c2, c3));
      } else { // Vertex coordinates
        str >> tmp >> c1 >> c2 >> c3;
        vertices.push_back(Vertex(c1, c2, c3));
      }
    } else if (curLine[0] == 'f') { // Faces
      str >> tmp;
      for (int i = 0; i < 3; ++i) {
        str >> vIndex[i] >> c >> c >> nIndex[i];
        vIndex[i]--; // OBJ indices start at 1, so convert to 0-based
        tIndex[i]--;
        nIndex[i]--;
      }
      assert(vIndex[0] == nIndex[0] && vIndex[1] == nIndex[1] &&
             vIndex[2] == nIndex[2]);
      faces.push_back(Face(vIndex, tIndex, nIndex));
    } else {
      std::cout << "Ignoring unidentified line in OBJ file: " << curLine
                << std::endl;
    }
  }
  myfile.close();

  // Check for vertex and normal count consistency
  if (vertices.size() != normals.size()) {
    std::cerr << "Mismatch between vertex and normal count." << std::endl;
    return false;
  }

  return true;
}

// Set up VBO, EBO, etc.
void Model::setupBuffers() {
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  assert(glGetError() == GL_NONE);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glGenBuffers(1, &normalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Normal), normals.data(),
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Normal), (void *)0);

  glGenBuffers(1, &textureBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
  glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(Texture),
               textures.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void *)0);

  std::vector<GLuint> indices;
  for (const Face &face : faces) {
    indices.insert(indices.end(),
                   {face.vIndex[0], face.vIndex[1], face.vIndex[2]});
  }
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
               indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}
void Model::draw(ShaderProgram &shaderProgram, float angle, Axis rotationAxis,
                 vec3 position, vec3 scale, Camera &camera) {
  // Ensure the shader program is bound before drawing
  // This should be the ID of your active shader program
  mat4 translationMatrix = translate(mat4(10.f), position);
  vec3 rotationVector = vec3(0, 0, 0);
  if (rotationAxis == X)
    rotationVector.x = 1;
  else if (rotationAxis == Y)
    rotationVector.y = 1;
  else
    rotationVector.z = 1;
  mat4 rotationMatrix =
      glm::rotate(mat4(1.f), glm::radians(angle), rotationVector);
  mat4 scaleMatrix = glm::scale(mat4(1.f), scale);
  mat4 modelMat = translationMatrix * rotationMatrix * scaleMatrix;

  shaderProgram.useProgram();
  shaderProgram.setUniform("modelingMat", modelMat);
  shaderProgram.setUniform("modelingMatInvTr",
                           glm::transpose(glm::inverse(modelMat)));
  shaderProgram.setUniform("perspectiveMat", camera.getProjectionMatrix());
  shaderProgram.setUniform("viewingMat", camera.getViewMatrix());
  // Bind the Vertex Array Object
  glBindVertexArray(vao);
  // Draw the model
  // The number of indices is the number of faces times 3 (since each face is a
  // triangle)
  glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);

  // Unbind the VAO to prevent accidental changes
  glBindVertexArray(0);

  // Optional: Check for any OpenGL errors
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error during drawing: " << gluErrorString(err)
              << std::endl;
  }
}

void Model::setPosition(const glm::vec3 &pos) { position = pos; }

vec3 Model::getPosition() const { return position; }
