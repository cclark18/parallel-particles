#ifndef __MESH_H
#define __MESH_H
#define TEX_SIZE 1024
#define MIPMAP_SIZE 512

#include <iostream>
#include "Handles.h"
#include "Library/tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr



class Mesh {
  public:
    // should be called once to initialize object
    // pass a string that names the .obj file to be loaded,
    // e.g. "cube.obj"
  void loadShapes(const std::string &objFile);
  void drawObject(Handles *handles);
  float radius;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::vector<float> norBuf;
  std::vector<float> posBuf;
  GLuint posBufObj;
  GLuint norBufObj;
  GLuint indBufObj;
  int material;
 private:
  void computeBound(void);
  void resize_obj(void);
  void sendBufs(void);
  glm::vec3 center;
  glm::vec3 dimensions;
};
#endif
