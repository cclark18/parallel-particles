#ifndef __MESH_H
#define __MESH_H
#include <iostream>
//#include "PhongHandles.h"
#include "Library/tiny_obj_loader.h"
#include "Library/GLSL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include "BuildSettings.h"

/*
 * Class that contains vertex position, face, normal, etc. data
 * Taken from 476 project for use in this project.
 */

class Mesh {
  public:
    // should be called once to initialize object
    // pass a string that names the .obj file to be loaded,
    // e.g. "cube.obj"
    void loadShapes(const std::string &objFile);
    float radius;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    GLuint posBufObj;
    GLuint norBufObj;
    GLuint indBufObj;
    int material;
  private:
    void computeBound(void);
    void resize_obj(void);
    void sendBufs(void);
    void computeNormals();
    glm::vec3 center;
    glm::vec3 dimensions;
};
#endif
