#include "../include/ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram() { programID = glCreateProgram(); }

ShaderProgram::~ShaderProgram() {
  // Detach and delete shaders
  for (auto &shader : shaders) {
    glDetachShader(programID, shader.second);
    glDeleteShader(shader.second);
  }
  // Delete the shader program
  glDeleteProgram(programID);
}

GLuint ShaderProgram::loadShader(const std::string &filename,
                                 GLenum shaderType) {
  std::string code;
  std::ifstream shaderFile(filename);
  if (!shaderFile) {
    std::cerr << "Unable to open shader file: " << filename << std::endl;
    return -1;
  }
  std::stringstream shaderStream;

  // Read file's buffer contents into stream
  shaderStream << shaderFile.rdbuf();
  shaderFile.close();
  code = shaderStream.str();

  GLuint shader = glCreateShader(shaderType);
  const char *shaderCode = code.c_str();
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);

  // Check for shader compile errors
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[1024] = {0};
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  return shader;
}

void ShaderProgram::attachShader(const std::string &filename,
                                 GLenum shaderType) {
  GLuint shader = loadShader(filename, shaderType);
  if (shader != 0) {
    glAttachShader(programID, shader);
    shaders[shaderType] = shader;
    std::cout << "Attached shader: " << filename << std::endl;
    // glDeleteShader(shader); // Flag shader for deletion
  }
}
void ShaderProgram::linkProgram() {
  glLinkProgram(programID);

  // Check for linking errors
  GLint success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[1024];
    glGetProgramInfoLog(programID, 1024, NULL, infoLog);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
}

void ShaderProgram::bindAttribute(GLuint location,
                                  const std::string &attributeName) {
  glBindAttribLocation(programID, location, attributeName.c_str());
}
void ShaderProgram::useProgram() { glUseProgram(programID); }

// Implementations of setUniform() methods ...

void ShaderProgram::setUniform(const string &name, float value) {
  glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
  // check for any openGL errors
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error during setting uniform1f: " << name
              << gluErrorString(err) << std::endl;
  }
}
void ShaderProgram::setUniform(const string &name, int value) {
  glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error during setting uniform1i: " << name
              << gluErrorString(err) << std::endl;
  }
}
void ShaderProgram::setUniform(const string &name, const vec3 &value) {
  glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1,
               value_ptr(value));
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error during setting uniform3fv: " << name
              << gluErrorString(err) << std::endl;
  }
}
void ShaderProgram::setUniform(const string &name, const mat4 &value) {
  useProgram();
  glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE,
                     value_ptr(value));
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error during setting uniformMx4fv: " << name
              << gluErrorString(err) << std::endl;
  }
}

GLuint ShaderProgram::getUniformLocation(const string &name) {
  return glGetUniformLocation(programID, name.c_str());
}
