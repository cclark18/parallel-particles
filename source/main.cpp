#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
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
#include "PhongHandles.h"
#include "PointHandles.h"
#include "Mesh.h"
#include "Camera.h"
#include "ParticleSystem.h"

#define FLAT_GRAY 0

GLFWwindow* window;
using namespace std;
using namespace glm;
int g_width = 1280;
int g_height = 720;
//vector<tinyobj::shape_t> shapes;
Camera camera;
double deltaTime;
double mouseSpeed = 200.0f;
float keySpeed = 4.0;

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
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
      move = (float)(keySpeed * deltaTime) * camera.getForward();
      break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
      move = (float)(-1 * keySpeed * deltaTime) * camera.getStrafe();
      break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
      move = (float)(-1 * keySpeed * deltaTime) * camera.getForward();
      break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
      move = (float)(keySpeed * deltaTime) * camera.getStrafe();
      break;
    }
    camera.eye += move;
    camera.lookat += move;
  }

  //cout << "eye: <" << camera.eye.x << ", " << camera.eye.y << ", " << camera.eye.z << ">" << endl;
  //cout << "lookat: <" << camera.lookat.x << ", " << camera.lookat.y << ", " << camera.lookat.z << ">" << endl;
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
  double x_center = g_width / 2.0;
  double y_center = g_height / 2.0;

  double dx = xpos - x_center;
  double dy = ypos - y_center;

  float maxMove = mouseSpeed * deltaTime;
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

void setPhongMaterial(PhongHandles *phongHandles, int mat)
{
  switch (mat) {
  case FLAT_GRAY:
    glUniform3f(phongHandles->uMatAmb, 0.1f, 0.1f, 0.1f);
    glUniform3f(phongHandles->uMatDif, 0.6f, 0.6f, 0.6f);
    glUniform3f(phongHandles->uMatSpec, 0.3f, 0.3f, 0.3f);
    glUniform1f(phongHandles->uMatShine, 2.0f);
    break;
  default:
    cout << "invalid material!" << endl;
  }
}

void moveParticles(ParticleSystem particles, int numMeshPoints, soa_point_t meshPos, soa_point_t meshNorm)
{
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

  PhongHandles phongHandles;
  phongHandles.installShaders("../resources/shaders/phongVert.glsl", "../resources/shaders/phongFrag.glsl");

  PointHandles pointHandles;
  pointHandles.installShaders("../resources/shaders/pointVert.glsl", "../resources/shaders/pointFrag.glsl");

  Mesh obj1;
  obj1.loadShapes("../resources/models/bunny.obj");

  ParticleSystem particleSystem;
  particleSystem.addMesh(&obj1);
  particleSystem.center = glm::vec3(0.0f, 0.0f, 0.0f);
  //particleSystem.center = glm::vec3(-1.2f, 0.0f, -0.2f);
  particleSystem.baseColor = glm::vec3(1.0f, 0.0f, 1.0f);

  glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
  glPointSize(5);

  do{
    TimeManager::Instance().CalculateFrameRate(true);
    deltaTime = TimeManager::Instance().DeltaTime;

    for (int i = 0; i < 64; ++i) {
      particleSystem.addParticle();
    }
    //particleSystem.addParticle();
    particleSystem.update(deltaTime);
    //cout << "num particles: " << particleSystem.particles.size() << endl;

    //glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDrawBuffer(GL_BACK);
    //glCullFace(GL_FRONT);
    //glDrawBuffer(GL_NONE);
    glCullFace(GL_BACK);
    glUseProgram(phongHandles.prog);
    setPhongMaterial(&phongHandles, FLAT_GRAY);
    glUniform3f(phongHandles.uLightPos, 3.0f, 15.0f, -4.0f);
    glUniform3f(phongHandles.uLightCol, 1.0f, 1.0f, 1.0f);
    glUniform3f(phongHandles.uCamPos, camera.eye.x, camera.eye.y, camera.eye.z);
    safe_glUniformMatrix4fv(phongHandles.uModelMatrix, glm::value_ptr(glm::mat4(1.0f)));
    safe_glUniformMatrix4fv(phongHandles.uViewMatrix, glm::value_ptr(camera.getView()));
    safe_glUniformMatrix4fv(phongHandles.uProjMatrix, glm::value_ptr(camera.getProjection()));
    phongHandles.draw(&obj1);

    glUseProgram(pointHandles.prog);
    safe_glUniformMatrix4fv(pointHandles.uViewMatrix, glm::value_ptr(camera.getView()));
    safe_glUniformMatrix4fv(pointHandles.uProjMatrix, glm::value_ptr(camera.getProjection()));
    pointHandles.draw(particleSystem.getPositions(), particleSystem.getColors());
    //cout << "num drawn: " << particleSystem.getPositions().size() / 3 << endl;

    glUseProgram(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window) == 0);

  glfwTerminate();
  return 0;
}
