#include "Mesh.h"
#define DEBUG

void Mesh::loadShapes(const std::string &objFile) {
  std::string err = tinyobj::LoadObj(this->shapes, this->materials, objFile.c_str());
  if(!err.empty()) {
    std::cerr << err << std::endl;
  }
  this->resize_obj();
  this->sendBufs();
  //this->computeBound();
}

void Mesh::computeBound() {
  glm::vec3 result = glm::vec3(0, 0, 0);
  float minX, minY, minZ;
  float maxX, maxY, maxZ;

  minX = minY = minZ = 1.1754E+38F;
  maxX = maxY = maxZ = -1.1754E+38F;

  for (size_t s = 0; s < this->shapes.size(); ++s) {
    for (size_t v = 0; v < this->shapes[s].mesh.positions.size() / 3; v++) {
      if(this->shapes[s].mesh.positions[3*v+0] < minX) minX = this->shapes[s].mesh.positions[3*v+0];
      if(this->shapes[s].mesh.positions[3*v+0] > maxX) maxX = this->shapes[s].mesh.positions[3*v+0];

      if(this->shapes[s].mesh.positions[3*v+1] < minY) minY = this->shapes[s].mesh.positions[3*v+1];
      if(this->shapes[s].mesh.positions[3*v+1] > maxY) maxY = this->shapes[s].mesh.positions[3*v+1];

      if(this->shapes[s].mesh.positions[3*v+2] < minZ) minZ = this->shapes[s].mesh.positions[3*v+2];
      if(this->shapes[s].mesh.positions[3*v+2] > maxZ) maxZ = this->shapes[s].mesh.positions[3*v+2];
    }
  }

  this->center.x = (minX + maxX) / 2.0;
  this->center.y = (minY + maxY) / 2.0;
  this->center.z = (minZ + maxZ) / 2.0;
  this->radius = glm::distance(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
}

void Mesh::resize_obj() {
  float minX, minY, minZ;
  float maxX, maxY, maxZ;
  float scaleX, scaleY, scaleZ;
  float shiftX, shiftY, shiftZ;
  float epsilon = 0.001;

  minX = minY = minZ = 1.1754E+38F;
  maxX = maxY = maxZ = -1.1754E+38F;

  //Go through all vertices to determine min and max of each dimension
  for (size_t i = 0; i < this->shapes.size(); i++) {
    for (size_t v = 0; v < this->shapes[i].mesh.positions.size() / 3; v++) {
      if(this->shapes[i].mesh.positions[3*v+0] < minX) minX = this->shapes[i].mesh.positions[3*v+0];
      if(this->shapes[i].mesh.positions[3*v+0] > maxX) maxX = this->shapes[i].mesh.positions[3*v+0];

      if(this->shapes[i].mesh.positions[3*v+1] < minY) minY = this->shapes[i].mesh.positions[3*v+1];
      if(this->shapes[i].mesh.positions[3*v+1] > maxY) maxY = this->shapes[i].mesh.positions[3*v+1];

      if(this->shapes[i].mesh.positions[3*v+2] < minZ) minZ = this->shapes[i].mesh.positions[3*v+2];
      if(this->shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = this->shapes[i].mesh.positions[3*v+2];
    }
  }
  //From min and max compute necessary scale and shift for each dimension
  float maxExtent, xExtent, yExtent, zExtent;
  xExtent = maxX-minX;
  yExtent = maxY-minY;
  zExtent = maxZ-minZ;
  if (xExtent >= yExtent && xExtent >= zExtent) {
    maxExtent = xExtent;
  }
  if (yExtent >= xExtent && yExtent >= zExtent) {
    maxExtent = yExtent;
  }
  if (zExtent >= xExtent && zExtent >= yExtent) {
    maxExtent = zExtent;
  }
  scaleX = 2.0 / maxExtent;
  shiftX = minX + (xExtent/ 2.0);
  scaleY = 2.0 / maxExtent;
  shiftY = minY + (yExtent / 2.0);
  scaleZ = 2.0/ maxExtent;
  shiftZ = minZ + (zExtent)/2.0;

  minX -= shiftX;
  minX *= scaleX;
  minY -= shiftY;
  minY *= scaleY;
  minZ -= shiftZ;
  minZ *= scaleZ;
  maxX -= shiftX;
  maxX *= scaleX;
  maxY -= shiftY;
  maxY *= scaleY;
  maxZ -= shiftZ;
  maxZ *= scaleZ;

  // set box dimensions/center according to max x, y, and z
  this->center = glm::vec3((maxX + minX) * 0.5, (maxY + minY) * 0.5, (maxZ + minZ) * 0.5);
  this->dimensions = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);

  //Go through all verticies shift and scale them
  for (size_t i = 0; i < this->shapes.size(); i++) {
    for (size_t v = 0; v < this->shapes[i].mesh.positions.size() / 3; v++) {
      this->shapes[i].mesh.positions[3*v+0] = (this->shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
      assert(this->shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
      assert(this->shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
      this->shapes[i].mesh.positions[3*v+1] = (this->shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
      assert(this->shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
      assert(this->shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
      this->shapes[i].mesh.positions[3*v+2] = (this->shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
      assert(this->shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
      assert(this->shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
    }
  }
}

void Mesh::sendBufs() {
  for (int s = 0; s < this->shapes.size(); ++s) {
    // Send the position array to the GPU
    const std::vector<float> &posBuf = this->shapes[s].mesh.positions;
    glGenBuffers(1, &this->posBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

    // Send the normal array to the GPU
    const std::vector<float> &norBuf = this->shapes[s].mesh.normals;
    glGenBuffers(1, &this->norBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, this->norBufObj);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

    // Send the index array to the GPU
    const std::vector<unsigned int> &indBuf = this->shapes[s].mesh.indices;
    glGenBuffers(1, &this->indBufObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLSL::checkVersion();
    assert(glGetError() == GL_NO_ERROR);
  }
}

// binds all mesh attributes and draws triangles
// uniforms for the object should be sent prior to this call
void Mesh::draw(Handles *handles)
{
  // Enable and bind position array for drawing
  GLSL::enableVertexAttribArray(handles->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
  glVertexAttribPointer(handles->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable and bind normal array for drawing
  GLSL::enableVertexAttribArray(handles->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, this->norBufObj);
  glVertexAttribPointer(handles->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

  for (int s = 0; s < this->shapes.size(); ++s) {
    // Bind index array for drawing
    int nIndices = (int)this->shapes[s].mesh.indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indBufObj);

    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
  }

  GLSL::disableVertexAttribArray(handles->aPosition);
  GLSL::disableVertexAttribArray(handles->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Disable and unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
