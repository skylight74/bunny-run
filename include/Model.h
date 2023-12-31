#ifndef MODEL_H
#define MODEL_H

#include "Camera.h"
#include "ShaderProgram.h"
#include <GL/glew.h>
#include <string>
#include <vector>

// define a type that is an enum for x,y,z axices
typedef enum { X, Y, Z } Axis;

class Model {
public:
  Model(const std::string &filename);
  void draw(ShaderProgram &shaderProgram, float, Axis, vec3, vec3,
            Camera &camera);
  void setPosition(const glm::vec3 &newPosition);
  glm::vec3 getPosition() const;

private:
  glm::vec3 position;
  struct Vertex {
    GLfloat x, y, z;
    Vertex(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
  };

  struct Texture {
    GLfloat u, v;
    Texture(GLfloat u, GLfloat v) : u(u), v(v) {}
  };

  struct Normal {
    GLfloat x, y, z;
    Normal(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
  };

  struct Face {
    GLuint vIndex[3], tIndex[3], nIndex[3];
    Face(int v[], int t[], int n[]) {
      for (int i = 0; i < 3; i++) {
        vIndex[i] = v[i];
        tIndex[i] = t[i];
        nIndex[i] = n[i];
      }
    }
  };

  std::vector<Vertex> vertices;
  std::vector<Texture> textures;
  std::vector<Normal> normals;
  std::vector<Face> faces;
  GLuint vertexBuffer, normalBuffer, textureBuffer, indexBuffer, vao;

  bool parseObj(const std::string &filename);
  void setupBuffers();
};

#endif // MODEL_H
