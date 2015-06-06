#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <time.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, txrans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <memory>

#include "Library/TimeManager.h"
#include "Library/InitObjects.h"
#include "Library/tiny_obj_loader.h"
#include "Library/GLSL.h"
#include "Library/GLError.h"
#include "Handles.h"
#include "Mesh.h"
#include "Camera.h"

#define FLAT_GRAY 0

GLFWwindow* window;
using namespace std;
using namespace glm;
int g_width = 1280;
int g_height = 720;
vector<tinyobj::shape_t> shapes;
Camera camera;
double deltaTime;
double camSpeed = 1.0f;
float key_speed = 2.0;

/* helper function to make sure your matrix handle is correct */
inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

void window_size_callback(GLFWwindow* window, int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  g_width = w;
  g_height = h;
  camera.aspect = (float)g_width / (float)g_height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  glm::vec3 move(0.0f, 0.0f, 0.0f);
  switch (key) {
  case GLFW_KEY_W:
  case GLFW_KEY_UP:
    move = (float)(key_speed * deltaTime) * camera.getForward();
    break;
  case GLFW_KEY_A:
  case GLFW_KEY_LEFT:
    move = (float)(key_speed * deltaTime) * camera.getStrafe();
    break;
  case GLFW_KEY_S:
  case GLFW_KEY_DOWN:
    move = (float)(-1 * key_speed * deltaTime) * camera.getForward();
    break;
  case GLFW_KEY_D:
  case GLFW_KEY_RIGHT:
    move = (float)(-1 * key_speed * deltaTime) * camera.getStrafe();
    break;
  }
  camera.eye += move;
  camera.lookat += move;
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
  double x_center = g_width / 2.0;
  double y_center = g_height / 2.0;

  double dx = xpos - x_center;
  double dy = ypos - y_center;

  float maxMove = camSpeed * deltaTime;
  if (dx > 0) {
    dx = dx < maxMove ? dx : maxMove;
  }
  else {
    dx = dx > -1.0 * maxMove ? dx : -1.0 * maxMove;
  }
  if (dy > 0) {
    dy = dy < maxMove ? dy : maxMove;
  }
  else {
    dy = dy > -1.0 * maxMove ? dy : -1.0 * maxMove;
  }
  camera.moveHoriz(-1.0 * dx * 0.01);
  camera.moveVert(dy * 0.01);

  glfwSetCursorPos(window, x_center, y_center);
}

void initGL()
{
  // Set the background color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // Enable Z-buffer test
  glEnable(GL_DEPTH_TEST);
  glPointSize(18);
}

void setMaterial(Handles *handles, int mat)
{
  switch (mat) {
  default:
  case FLAT_GRAY:
    glUniform3f(handles->uMatAmb, 0.1f, 0.1f, 0.1f);
    glUniform3f(handles->uMatDif, 0.6f, 0.6f, 0.6f);
    glUniform3f(handles->uMatSpec, 0.3f, 0.3f, 0.3f);
    glUniform1f(handles->uMatShine, 2.0f);
    break;
  }
}

int main(int argc, char **argv)
{
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // Open a window and create its OpenGL context
  printf("%d, %d\n", g_width, g_height);
  window = glfwCreateWindow(g_width, g_height, "Parallel Particles", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetWindowSizeCallback(window, window_size_callback);
  // Initialize GLAD
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSetCursorPos(window, g_width / 2, g_height / 2);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  initGL();
  assert(glGetError() == GL_NO_ERROR);

  srand(time(NULL));

  Handles handles;
  handles.installShaders("../resources/shaders/phongVert.glsl", "../resources/shaders/phongFrag.glsl");
  Mesh obj1;
  obj1.loadShapes("../resources/models/bunny.obj");

  glClearColor(0.0f, 0.0f, 0.3f, 1.0f);

  do{
    TimeManager::Instance().CalculateFrameRate(true);
    deltaTime = TimeManager::Instance().DeltaTime;

    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDrawBuffer(GL_BACK);
    glCullFace(GL_BACK);
    glUseProgram(handles.prog);
    glUniform3f(handles.uLightPos, 3.0f, 15.0f, -4.0f);
    glUniform3f(handles.uLightCol, 1.0f, 1.0f, 1.0f);
    glUniform3f(handles.uCamPos, camera.eye.x, camera.eye.y, camera.eye.z);
    setMaterial(&handles, FLAT_GRAY);
    safe_glUniformMatrix4fv(handles.uModelMatrix, glm::value_ptr(glm::mat4(1.0f)));
    obj1.draw(&handles);
    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window) == 0);

  glfwTerminate();
  return 0;
}
