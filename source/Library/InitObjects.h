#ifndef InitObjects_H_
#define InitObjects_H_

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include "GLSL.h"
#include "tiny_obj_loader.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr

using namespace std;
using namespace glm;

void initModelObject(std::vector<tinyobj::shape_t>& shape, GLuint* posBuffer,
    GLuint* norBuffer, GLuint* indBuffer);

void initVertexObject(GLuint* posBuffer, GLuint* norBuffer, GLuint* idxBuffer, GLuint* texBuffer, GLfloat* vertexData, GLfloat* normalData, GLint numVertices);

void resize_obj(std::vector<tinyobj::shape_t> &shapes);

void loadShapes(const string &objFile, std::vector<tinyobj::shape_t>& shapes,
	vector<tinyobj::material_t> materials);

#endif
