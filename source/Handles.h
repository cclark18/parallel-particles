//
//  Handles.h
//

#ifndef __MarchingCubes__Handles__
#define __MarchingCubes__Handles__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Library/GLSL.h"

class Handles {
public:
   GLuint prog;
   GLuint aPosition;
   GLuint aNormal;
   GLuint uLightPos;
   GLuint uMatAmb;
   GLuint uMatDif;
   GLuint uMatSpec;
   GLuint uMatShine;
   GLuint uProjMatrix;
   GLuint uViewMatrix;
   GLuint uModelMatrix;
   GLuint uCamPos;
   bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
};

#endif /* defined(__MarchingCubes__Handles__) */
