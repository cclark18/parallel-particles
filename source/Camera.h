#ifndef CAMERA_H_
#define CAMERA_H_

/*
 * Basic camera class. Controlled by directly modifying eye, lookat, etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <memory>
#include <cmath>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

#define PI 3.1415926535897932384626f
#define DEFAULT_FOV 60.0f
#define DEFAULT_ASPECT 1.777778f
#define DEFAULT_NEAR 0.1f
#define DEFAULT_FAR 30.0f

class Camera {
  public:
    //Properties
    glm::vec3 lookat;
    glm::vec3 eye;
    glm::vec3 up;
    float fov;
    float aspect;
    float _near;
    float _far;
    float max_vert_angle = 85;

    //Constructor
    Camera();
    Camera(glm::vec3 lookat, glm::vec3 eye, glm::vec3 up, float fov, float aspect, float _near, float _far);

    // these vectors are all normalized
    virtual glm::vec3 getForward();
    virtual glm::vec3 getStrafe();
    virtual glm::vec3 getUp();

    // gets view/proj matrices
    virtual glm::mat4 getView();
    virtual glm::mat4 getProjection();

    virtual void moveVert(float step);
    virtual void moveHoriz(float step);

  private:
    float theta = 0;
    float phi = 0;
};

#endif
