﻿#include <iostream>
#include <cmath>
#include <BlackBox/ObjLoader.hpp>
#include <BlackBox/ObjLoader.hpp>
#include <BlackBox/IGeometry.hpp>
#include <cstring>
#include <cstdio>
#include <string>

#include <glad/glad.h>

#include <vector>
#include <glm/glm.hpp>
#include <BlackBox/IGeometry.hpp>

bool loadOBJ(
  const char * path,
  std::vector <Vertex> & out_vertices
);


// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h> 
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
  // Init GLFW
  glfwInit();
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // GLFW Options
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  /*
  // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  glewInit();
  */
  if (!gladLoadGL())
    return -1;
  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  // OpenGL options
  glEnable(GL_DEPTH_TEST);


  // Build and compile our shader program
  Shader lightingShader("light.vs", "light.frag");
  Shader lampShader("lamp.vs", "lamp.frag");

  // Set up vertex data (and buffer(s)) and attribute pointers
   std::vector<Vertex> BMW;
  if (!loadOBJ("bmw.obj", BMW))
  {
    return -1;
  }
  // First, set the container's VAO (and VBO)
  GLuint VBO, containerVAO;
  glGenVertexArrays(1, &containerVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * BMW.size(), BMW.data(), GL_STATIC_DRAW);

  glBindVertexArray(containerVAO);
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);
  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, n));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Set the vertex attributes (only position data for the lamp))
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // Note that we skip over the normal vectors
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);


  // Game loop
  while (!glfwWindowShouldClose(window))
  {
    // Calculate deltatime of current frame
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();
    do_movement();

    // Clear the colorbuffer
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use cooresponding shader when setting uniforms/drawing objects
    lightingShader.Use();
    GLint objectColorLoc = glGetUniformLocation(lightingShader.Program, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(lightingShader.Program, "lightColor");
    GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "lightPos");
    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);


    // Create camera transformations
    glm::mat4 view;
    view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
    GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
    GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
    // Pass the matrices to the shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the container (using container's vertex attributes)
    glBindVertexArray(containerVAO);
    glm::mat4 model(1.0);
    model = glm::scale(model, glm::vec3(3, 3, 3));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, BMW.size());
    glBindVertexArray(0);

    // Also draw the lamp object, again binding the appropriate shader
    lampShader.Use();
    // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
    modelLoc = glGetUniformLocation(lampShader.Program, "model");
    viewLoc = glGetUniformLocation(lampShader.Program, "view");
    projLoc = glGetUniformLocation(lampShader.Program, "projection");
    // Set matrices
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    model = glm::mat4(1.0);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Draw the light object (using light's vertex attributes)
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();
  return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if (key >= 0 && key < 1024)
  {
    if (action == GLFW_PRESS)
      keys[key] = true;
    else if (action == GLFW_RELEASE)
      keys[key] = false;
  }
}

void do_movement()
{
  // Camera controls
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  GLfloat xoffset = xpos - lastX;
  GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}



bool loadOBJ(const char * path, std::vector <Vertex> & out_vertices)
{
  std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
  std::vector< glm::vec3 > temp_vertices;
  std::vector< glm::vec2 > temp_uvs;
  std::vector< glm::vec3 > temp_normals;
  bool has_uv = false;
  int f_cnt = 0;
  int v_cnt = 0;
  unsigned int current_face = 0;

  FILE * file = fopen(path, "r");
  if (file == NULL) {
    printf("Impossible to open the file !\n");
    return false;
  }
  while (1) {

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
      break; // EOF = End Of File. Quit the loop.

    // else : parse lineHeader
    if (strcmp(lineHeader, "v") == 0) {
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
      temp_vertices.push_back(vertex);
    }
    else if (strcmp(lineHeader, "vt") == 0) {
      has_uv = true;
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y);
      temp_uvs.push_back(uv);
    }
    else if (strcmp(lineHeader, "vn") == 0) {
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
      temp_normals.push_back(normal);
    }
    else if (strcmp(lineHeader, "f") == 0) {
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
      int cnt = 6;
      const char *pattern[2] = {
        "%d/%d/%d %d/%d/%d %d/%d/%d\n",
        "%d//%d %d//%d %d//%d\n",
      };
      if (has_uv) cnt = 9;
      int matches;
      if (has_uv)
        matches = fscanf(file, pattern[0],
          &vertexIndex[0], &uvIndex[0], &normalIndex[0],
          &vertexIndex[1], &uvIndex[1], &normalIndex[1],
          &vertexIndex[2], &uvIndex[2], &normalIndex[2]
        );
      else
        matches = fscanf(file, pattern[1],
          &vertexIndex[0], &normalIndex[0],
          &vertexIndex[1], &normalIndex[1],
          &vertexIndex[2], &normalIndex[2]
        );

      if (matches != cnt) {
        printf("File can't be read by our simple parser : ( Try exporting with other options\n");
        return false;
      }
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);
      if (has_uv)
      {
        uvIndices.push_back(uvIndex[0]);
        uvIndices.push_back(uvIndex[1]);
        uvIndices.push_back(uvIndex[2]);
      }
      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);

      // For each vertex of each triangle
      Vertex _vertex;
      //for (unsigned int i = 0; i < vertexIndices.size(); i++) {
      for (; current_face < vertexIndices.size(); current_face++) {
        unsigned int vertexIndex = vertexIndices[current_face];
        unsigned int normalIndex = normalIndices[current_face];

        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        _vertex.pos = vertex;

        if (has_uv)
        {
          unsigned int uvIndex = uvIndices[current_face];
          glm::vec2 uv = temp_uvs[uvIndex - 1];
          _vertex.uv = uv;
        }

        glm::vec3 normal = temp_normals[normalIndex - 1];
        _vertex.n = normal;
        out_vertices.push_back(_vertex);
      }
    }
    else {
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }
  }

  return true;
}