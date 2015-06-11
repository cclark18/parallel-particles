//
//  PointHandles.h
//  
//  Contains handles for data sent to point shader,
//  and handles shader loading and mesh drawing
//
//  Modified from 476 project for use in this project
//

#ifndef __PointHandles
#define __PointHandles

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Library/GLSL.h"
#include "Mesh.h"

class PointHandles {
  public:
    GLuint prog;
    GLuint aPosition;
    GLuint aColor;
    GLuint uViewMatrix;
    GLuint uProjMatrix;
    bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
    void draw(std::vector<float> positions, std::vector<float> colors);
};

#endif
