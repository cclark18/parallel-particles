#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
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
#include "BuildSettings.h"

#define FLAT_GRAY 0
#define MOVE_RANGE 2
#define PARTS_PER_SEC_DEFAULT 10

using namespace std;
using namespace glm;

double deltaTime;
#ifdef RENDER
GLFWwindow* window;
int g_width = 1280;
int g_height = 720;
Camera camera;
double mouseSpeed = 200.0f;
float keySpeed = 4.0;

/* helper function to make sure your matrix handle is correct */
inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

// handles window resizing
void window_size_callback(GLFWwindow* window, int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  g_width = w;
  g_height = h;
  camera.aspect = (float)g_width / (float)g_height;
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
#endif

int main(int argc, char **argv)
{
#ifdef RENDER
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
  glfwSetWindowSizeCallback(window, window_size_callback);
  // Initialize GLAD
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSetCursorPos(window, g_width / 2, g_height / 2);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  initGL();
  assert(glGetError() == GL_NO_ERROR);

  // set back color and point size
  glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
  glPointSize(5);

  // load shaders
  PhongHandles phongHandles;
  phongHandles.installShaders("../resources/shaders/phongVert.glsl", "../resources/shaders/phongFrag.glsl");

  PointHandles pointHandles;
  pointHandles.installShaders("../resources/shaders/pointVert.glsl", "../resources/shaders/pointFrag.glsl");
#endif

  // load mesh from .obj file
  Mesh obj1;
  obj1.loadShapes("../resources/models/bunny.obj");

  // initialize particle system
  ParticleSystem particleSystem;
  particleSystem.addMesh(&obj1);
  particleSystem.center = glm::vec3(0.0f, 1.0f, 0.0f);
  particleSystem.baseColor = glm::vec3(0.949f, 0.337f, 0.133f);

  // setup for mesh animation
  glm::vec3 objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
  bool moveRight = true;

  // read command line arguments (if present) to set particle spawn rate and lifetime
  int partsPerSec = PARTS_PER_SEC_DEFAULT;
  if (argc > 1) {
    partsPerSec = atoi(argv[1]);
  }
  if (argc > 2) {
    particleSystem.maxAge = atoi(argv[2]);
  }


  std::chrono::time_point<std::chrono::system_clock> last, cur, start, end;
  cur = std::chrono::system_clock::now();

  do{
    last = cur;
    cur = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = cur-last;
    deltaTime = elapsed_seconds.count();

    // clunky "bunny hop" animation
    if (moveRight) {
      objCenter.x += deltaTime;
    }
    else {
      objCenter.x -= deltaTime;
    }
    if (objCenter.x > MOVE_RANGE) {
      moveRight = false;
    }
    if (objCenter.x < -1 * MOVE_RANGE) {
      moveRight = true;
    }
    objCenter.y = 0.6 * abs(sin(objCenter.x * 3));
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), objCenter);
    
    start = std::chrono::system_clock::now();

    // run particle system
    particleSystem.addParticles((int)(deltaTime * 60.0 * partsPerSec));
    particleSystem.update(deltaTime, translate);

    vector<float> partPositions = particleSystem.getPositions();
    vector<float> partColors = particleSystem.getColors();

    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    cout << "Particle calculation time: " << elapsed_seconds.count() << "s (" << partPositions.size() / 3 << " particles)" << endl;

#ifdef RENDER
    // render mesh w/ phong lighting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDrawBuffer(GL_BACK);
    glCullFace(GL_BACK);
    glUseProgram(phongHandles.prog);
    setPhongMaterial(&phongHandles, FLAT_GRAY);
    glUniform3f(phongHandles.uLightPos, 3.0f, 15.0f, -4.0f);
    glUniform3f(phongHandles.uLightCol, 1.0f, 1.0f, 1.0f);
    glUniform3f(phongHandles.uCamPos, camera.eye.x, camera.eye.y, camera.eye.z);
    safe_glUniformMatrix4fv(phongHandles.uModelMatrix, glm::value_ptr(translate));
    safe_glUniformMatrix4fv(phongHandles.uViewMatrix, glm::value_ptr(camera.getView()));
    safe_glUniformMatrix4fv(phongHandles.uProjMatrix, glm::value_ptr(camera.getProjection()));
    phongHandles.draw(&obj1);

    // render particles from particle system
    glUseProgram(pointHandles.prog);
    safe_glUniformMatrix4fv(pointHandles.uViewMatrix, glm::value_ptr(camera.getView()));
    safe_glUniformMatrix4fv(pointHandles.uProjMatrix, glm::value_ptr(camera.getProjection()));
    pointHandles.draw(partPositions, partColors);

    // end of frame housekeeping
    glUseProgram(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window) == 0);
#else
  } while (true);
#endif

#ifdef RENDER
  glfwTerminate();
#endif
  return 0;
}
