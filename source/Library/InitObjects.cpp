#include "InitObjects.h"

void initModelObject(std::vector<tinyobj::shape_t>& shape, GLuint* posBuffer,
    GLuint* norBuffer, GLuint* indBuffer) {

    // Send the position array to the GPU
    const vector<float> &posBuf = shape[0].mesh.positions;
    glGenBuffers(1, posBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *posBuffer);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	const vector<float> &norBuf = shape[0].mesh.normals;
	glGenBuffers(1, norBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *norBuffer);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	/**
    // Send the normal array to the GPU
    vector<float> norBuf;
    glm::vec3 v1, v2, v3;
    glm::vec3 edge1, edge2, norm;
    int idx1, idx2, idx3;
    //for every vertex initialize the vertex normal to 0
    for (unsigned int j = 0; j < shape[0].mesh.positions.size() / 3; j++) {
        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(0);
    }
    //process the mesh and compute the normals - for every face
    //add its normal to its associated vertex
    for (unsigned int i = 0; i < shape[0].mesh.indices.size() / 3; i++) {
        idx1 = shape[0].mesh.indices[3 * i + 0];
        idx2 = shape[0].mesh.indices[3 * i + 1];
        idx3 = shape[0].mesh.indices[3 * i + 2];
        v1 = glm::vec3(shape[0].mesh.positions[3 * idx1 + 0], 
            shape[0].mesh.positions[3 * idx1 + 1], 
            shape[0].mesh.positions[3 * idx1 + 2]);
        v2 = glm::vec3(shape[0].mesh.positions[3 * idx2 + 0], 
            shape[0].mesh.positions[3 * idx2 + 1], 
            shape[0].mesh.positions[3 * idx2 + 2]);
        v3 = glm::vec3(shape[0].mesh.positions[3 * idx3 + 0], 
            shape[0].mesh.positions[3 * idx3 + 1], 
            shape[0].mesh.positions[3 * idx3 + 2]);
        edge1 = v2 - v1;
        edge2 = v3 - v1;
        norm = glm::cross(edge1, edge2);
        norBuf[3 * idx1 + 0] += (norm.x);
        norBuf[3 * idx1 + 1] += (norm.y);
        norBuf[3 * idx1 + 2] += (norm.z);
        norBuf[3 * idx2 + 0] += (norm.x);
        norBuf[3 * idx2 + 1] += (norm.y);
        norBuf[3 * idx2 + 2] += (norm.z);
        norBuf[3 * idx3 + 0] += (norm.x);
        norBuf[3 * idx3 + 1] += (norm.y);
        norBuf[3 * idx3 + 2] += (norm.z);
    }
    glGenBuffers(1, norBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *norBuffer);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	*/
    // Send the index array to the GPU
    const vector<unsigned int> &indBuf = shape[0].mesh.indices;
    glGenBuffers(1, indBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLSL::checkVersion();
    assert(glGetError() == GL_NO_ERROR);
}

void initVertexObject(GLuint* posBuffer, GLuint* norBuffer, GLuint* idxBuffer, GLuint* texBuffer, GLfloat* vertexData, 
		      GLfloat* normalData, int numVertices) {
  glGenBuffers(1, posBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, *posBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData) * numVertices * 3, vertexData, GL_STATIC_DRAW);

  glGenBuffers(1, norBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, *norBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normalData) * numVertices * 3, normalData, GL_STATIC_DRAW);

  unsigned short idx[] = {0, 1, 2, 1, 2, 3};

  static GLfloat groundTex[] = {
    0, 0,
    0, 100,
    100, 100,
    0, 100, 
    100, 100, 
    100, 0
  };

  glGenBuffers(1, texBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, *texBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(groundTex), groundTex, GL_STATIC_DRAW);
  
  glGenBuffers(1, idxBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *idxBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}


void resize_obj(std::vector<tinyobj::shape_t> &shapes){
    //Given a vector of shapes which has already been read from an obj file
    // resize all vertices to the range [-1, 1]
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001f;

    minX = minY = minZ = 1.1754E+38F;
    maxX = maxY = maxZ = -1.1754E+38F;

    //Go through all vertices to determine min and max of each dimension
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
            if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

            if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
            if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

            if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
            if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
        }
    }
    //From min and max compute necessary scale and shift for each dimension
    float maxExtent, xExtent, yExtent, zExtent;
    xExtent = maxX - minX;
    yExtent = maxY - minY;
    zExtent = maxZ - minZ;
    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }
    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }
    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }
    scaleX = 2.0f / maxExtent;
    shiftX = minX + (xExtent / 2.0f);
    scaleY = 2.0f / maxExtent;
    shiftY = minY + (yExtent / 2.0f);
    scaleZ = 2.0f / maxExtent;
    shiftZ = minZ + (zExtent) / 2.0f;

    //Go through all verticies shift and scale them
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
            assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
            assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
            assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
        }
    }
}

void loadShapes(const string &objFile, std::vector<tinyobj::shape_t>& shapes, 
    vector<tinyobj::material_t> materials)
{
    string err = tinyobj::LoadObj(shapes, materials, objFile.c_str());
    if (!err.empty()) {
        cerr << err << endl;
    }
    resize_obj(shapes);
}
