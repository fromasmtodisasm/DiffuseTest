#pragma once
#include <BlackBox/Opengl.hpp>
#include <Shader.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CObject
{
  GLuint VAO, VBO;
  glm::mat4 model;
  Shader* shader;
  const char* path;
  GLuint size;

  GLint modelLoc;
  GLint viewLoc;
  GLint projLoc;



  CObject();
  CObject(const char *path, Shader *shader);
  ~CObject();

  bool load();
  void draw();
};

