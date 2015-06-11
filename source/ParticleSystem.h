// ParticleSystem.h
//
// Handles particle generation and motion.
// All parallelism for this project occurs in this class.
// Converts particle data to OpenGL-friendly format after calculations.

#ifndef __PARTICLE_SYSTEM_H
#define __PARTICLE_SYSTEM_H
#include "SOAPoint.h"
#include "Particle.h"
#include "Mesh.h"
#include "BuildSettings.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <vector>
#include <cassert>

// constant multiplier for electrostatic charges
#define FORCE_CONSTANT 0.000001f

class ParticleSystem
{
  public:
    ParticleSystem();
    ~ParticleSystem();
    size_t numParticles;
    glm::vec3 center;
    glm::vec3 baseColor;
    void addParticles(int num);
    void addMesh(Mesh *mesh);
    void update(float step, glm::mat4 meshTransform);

    // get values in opengl-friendly format
    std::vector<float> getPositions();
    std::vector<float> getColors();

    void getSOAPositions(soa_point_t *result);
    void setSOAPositions(soa_point_t values);
    void getSOAMeshes(soa_point_t *result);

    //const size_t maxParticles = 500;
    float maxAge; // time in seconds
    bool offload;
    std::vector<Particle> particles;
  private:
    std::vector<Mesh*> meshes;
    soa_point_t particlePositionsOld;
    soa_point_t particlePositionsNew;
    soa_point_t meshPositions;
    const int partIncr = 4096;  // determines when new memory is allocated for adding particles
};

void calculate(size_t numParts,
               soa_point_t particles,
               soa_point_t meshPoints,
               soa_point_t out,
               float meshTransform[4][4],
               float step);
void allocate_soa(soa_point_t *point, size_t size);
void free_soa(soa_point_t *point);

#endif
