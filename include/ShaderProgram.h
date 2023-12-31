#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

using namespace std;
using namespace glm;

class ShaderProgram {
public:
  ShaderProgram();
  ~ShaderProgram();

  void attachShader(const std::string &filename, GLenum shaderType);
  void linkProgram();
  void useProgram();

  void setUniform(const string &name, float value);
  void setUniform(const string &name, int value);
  void setUniform(const string &name, const vec3 &value);
  void setUniform(const string &name, const mat4 &value);
  void bindAttribute(GLuint location, const std::string &attributeName);
  GLuint getUniformLocation(const string &name);

private:
  GLuint programID;
  unordered_map<GLenum, GLuint> shaders;

  GLuint loadShader(const string &filename, GLenum shaderType);
};

#endif // SHADERPROGRAM_H
