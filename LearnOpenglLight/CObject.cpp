#include "CObject.h"
#include <BlackBox/IGeometry.hpp>
#include <BlackBox/ObjLoader.hpp>



CObject::CObject()
{
}

CObject::CObject(const char* path, Shader *shader) : path(path), shader(shader)
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  // Get the uniform locations
  modelLoc = glGetUniformLocation(shader->Program, "model");
  viewLoc = glGetUniformLocation(shader->Program, "view");
  projLoc = glGetUniformLocation(shader->Program, "projection");
}


CObject::~CObject()
{
}

bool CObject::load()
{
  std::vector<Vertex> verts;
  
  if (!loadOBJ(path, verts))
  {
    return false;
  }
  size = verts.size();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

  glBindVertexArray(VAO);
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);
  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, n));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  return true;

}

void CObject::draw()
{

  // Draw the container (using container's vertex attributes)
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, size);
  glBindVertexArray(0);
}
