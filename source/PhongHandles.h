//
//  PhongHandles.h
//

#ifndef __MarchingCubes__PhongHandles
#define __MarchingCubes__PhongHandles

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Library/GLSL.h"
#include "Mesh.h"

class PhongHandles {
  public:
    GLuint prog;
    GLuint aPosition;
    GLuint aNormal;
    GLuint uLightPos;
    GLuint uLightCol;
    GLuint uMatAmb;
    GLuint uMatDif;
    GLuint uMatSpec;
    GLuint uMatShine;
    GLuint uProjMatrix;
    GLuint uViewMatrix;
    GLuint uModelMatrix;
    GLuint uCamPos;
    bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
    void draw(Mesh* mesh);
};

#endif /* defined(__MarchingCubes__PhongHandles) */
