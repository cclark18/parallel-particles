#include "ParticleSystem.h"
#define ALLOC alloc_if(1)
#define FREE free_if(1)
#define REUSE alloc_if(0)
#define RETAIN free_if(0)

ParticleSystem::ParticleSystem()
{
  baseColor = glm::vec3(0.0f, 0.5f, 0.9f);
  allocate_soa(&particlePositionsOld, partIncr);
  allocate_soa(&particlePositionsNew, partIncr);
  meshPositions.size = 0;
  srand(time(NULL));
  particlePositionsOld.size = 0;
  particlePositionsNew.size = 0;
  meshPositions.size = 0;
  maxAge = 5;
}

ParticleSystem::~ParticleSystem()
{
  free_soa(&particlePositionsOld);
  free_soa(&particlePositionsNew);
  free_soa(&meshPositions);
}

void ParticleSystem::update(float step, glm::mat4 meshTransform)
{
  // update time values, discard particles that have aged completely
  auto iter = particles.begin();
  while (iter != particles.end()) {
    iter->age += step;
    if (iter->age > maxAge) {
      iter = particles.erase(iter);
    }
    else {
      ++iter;
    }
  }

  float transformArray[4][4];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      transformArray[i][j] = meshTransform[i][j];
    }
  }

  // move particles
  getSOAPositions(&particlePositionsOld);
  assert(particlePositionsOld.size == particlePositionsNew.size);
  calculate(particles.size(), particlePositionsOld, meshPositions, particlePositionsNew, transformArray, step);
  setSOAPositions(particlePositionsNew);
}

void ParticleSystem::addParticles(int num)
{
  for (int i = 0; i < num; ++i) {
    Particle particle;
    particle.age = 0;
    float x = (rand() % 100 - 50) / 8000.0f;
    float y = (rand() % 100 - 50) / 8000.0f;
    float z = (rand() % 100 - 50) / 8000.0f;
    particle.position = center + glm::vec3(x, y, z);

    particles.push_back(particle);
  }

  while (particlePositionsOld.size <= particles.size()) {
    int oldSize = particlePositionsOld.size;
    free_soa(&particlePositionsOld);
    allocate_soa(&particlePositionsOld, oldSize + partIncr);
    free_soa(&particlePositionsNew);
    allocate_soa(&particlePositionsNew, oldSize + partIncr);
  }
}

// don't call this function often; always performs allocation
void ParticleSystem::addMesh(Mesh *mesh)
{
  meshes.push_back(mesh);

  size_t size = meshPositions.size;
  free_soa(&meshPositions);

  for (size_t s = 0; s < mesh->shapes.size(); ++s) {
    size += mesh->shapes[s].mesh.positions.size() / 3;
  }

  allocate_soa(&meshPositions, size);
  getSOAMeshes(&meshPositions);
}

void ParticleSystem::getSOAPositions(soa_point_t *result)
{
#ifdef PARALLEL
#pragma omp parallel for
#endif
  for (int i = 0; i < particles.size(); ++i) {
    result->x[i] = particles[i].position.x;
    result->y[i] = particles[i].position.y;
    result->z[i] = particles[i].position.z;
  }
}

void ParticleSystem::setSOAPositions(soa_point_t values)
{
#ifdef PARALLEL
#pragma omp parallel for
#endif
  for (int i = 0; i < particles.size(); ++i) {
    particles[i].position.x = values.x[i];
    particles[i].position.y = values.y[i];
    particles[i].position.z = values.z[i];
  }
}

void ParticleSystem::getSOAMeshes(soa_point_t *result)
{
  int index = 0;
  for (auto iter = meshes.begin(); iter != meshes.end(); ++iter) {
    for (size_t s = 0; s < (*iter)->shapes.size(); ++s) {
      for (int i = 0; i < (*iter)->shapes[s].mesh.positions.size()/3; i++) {
        result->x[index] = (*iter)->shapes[s].mesh.positions[3*i+0];
        result->y[index] = (*iter)->shapes[s].mesh.positions[3*i+1];
        result->z[index] = (*iter)->shapes[s].mesh.positions[3*i+2];
        ++index;
      }
    }
  }

#ifdef OFFLOAD_BUILD
  float *meshPosX = result->x;
  float *meshPosY = result->y;
  float *meshPosZ = result->z;
#pragma offload target(mic:0) \
  in (meshPosX:length(result->size) ALLOC RETAIN) \
  in (meshPosY:length(result->size) ALLOC RETAIN) \
  in (meshPosZ:length(result->size) ALLOC RETAIN)
  {
  }
#endif

}

// parallelism happens here
void calculate(size_t numParts,
               soa_point_t positions,
               soa_point_t meshPoints,
               soa_point_t out,
               float meshTransform[4][4],
               float step)
{
  float *partPosX = positions.x;
  float *partPosY = positions.y;
  float *partPosZ = positions.z;
  
  float *meshPosX = meshPoints.x;
  float *meshPosY = meshPoints.y;
  float *meshPosZ = meshPoints.z;

  float *outX = out.x;
  float *outY = out.y;
  float *outZ = out.z;

#ifdef VECTOR
  __assume_aligned(partPosX, 64);
  __assume_aligned(partPosY, 64);
  __assume_aligned(partPosZ, 64);
  __assume_aligned(meshPosX, 64);
  __assume_aligned(meshPosY, 64);
  __assume_aligned(meshPosZ, 64);
  __assume_aligned(outX, 64);
  __assume_aligned(outY, 64);
  __assume_aligned(outZ, 64);
#endif

#ifdef OFFLOAD_BUILD
#pragma offload target(mic:0) if (numParts > 0)\
  in (numParts) \
  in (meshTransform:length(16)) \
  in (meshPosX:length(meshPoints.size) REUSE RETAIN) \
  in (meshPosY:length(meshPoints.size) REUSE RETAIN) \
  in (meshPosZ:length(meshPoints.size) REUSE RETAIN) \
  out (outX:length(numParts) ALLOC FREE) \
  out (outY:length(numParts) ALLOC FREE) \
  out (outZ:length(numParts) ALLOC FREE) \
  in (partPosX:length(numParts) ALLOC FREE) \
  in (partPosY:length(numParts) ALLOC FREE) \
  in (partPosZ:length(numParts) ALLOC FREE)
#endif
  {

#ifdef PARALLEL
#ifdef VECTOR
#pragma omp parallel for simd
#else
#pragma omp parallel for
#endif
#endif
    for (size_t i = 0; i < numParts; ++i) {
      float dx = 0;
      float dy = 0;
      float dz = 0;
      // move away from other particles
      for (size_t j = 0; j < numParts; ++j) {
        float distx = partPosX[i] - partPosX[j];
        float disty = partPosY[i] - partPosY[j];
        float distz = partPosZ[i] - partPosZ[j];
        float distTot = sqrt(distx * distx + disty * disty + distz * distz + 0.000000000001);  // offset to avoid problems dividing by zero
        float force = 2.0 * step * FORCE_CONSTANT / (distTot * distTot * distTot);
        dx += distx * force;
        dy += disty * force;
        dz += distz * force;
      }

      // move away from mesh points
      for (size_t j = 0; j < meshPoints.size; ++j) {
        float meshFinalX = meshTransform[0][0] * meshPosX[j] +
                           meshTransform[1][0] * meshPosY[j] +
                           meshTransform[2][0] * meshPosZ[j] +
                           meshTransform[3][0];
        float meshFinalY = meshTransform[0][1] * meshPosX[j] +
                           meshTransform[1][1] * meshPosY[j] +
                           meshTransform[2][1] * meshPosZ[j] +
                           meshTransform[3][1];
        float meshFinalZ = meshTransform[0][2] * meshPosX[j] +
                           meshTransform[1][2] * meshPosY[j] +
                           meshTransform[2][2] * meshPosZ[j] +
                           meshTransform[3][2];
        float distx = partPosX[i] - meshFinalX;
        float disty = partPosY[i] - meshFinalY;
        float distz = partPosZ[i] - meshFinalZ;
        float distTot = sqrt(distx * distx + disty * disty + distz * distz + 0.000000000001);  // offset to avoid problems dividing by zero
        float force = 90.0 * step * FORCE_CONSTANT / (distTot * distTot * distTot);
        dx += distx * force;
        dy += disty * force;
        dz += distz * force;
      }

      // small "gravity" on particles
      dy -= 0.003;
      // apply translations
      outX[i] = partPosX[i] + dx;
      outY[i] = partPosY[i] + dy;
      outZ[i] = partPosZ[i] + dz;
    }
  }
}

std::vector<float> ParticleSystem::getPositions()
{
  std::vector<float> result;
  result.clear();

  for (auto iter = particles.begin(); iter != particles.end(); ++iter) {
    float x = iter->position.x;
    float y = iter->position.y;
    float z = iter->position.z;
    result.push_back(x);
    result.push_back(y);
    result.push_back(z);
  }

  return result;
}

std::vector<float> ParticleSystem::getColors()
{
  std::vector<float> result;

  for (auto iter = particles.begin(); iter != particles.end(); ++iter) {
    glm::vec3 color = baseColor * (1 - iter->age / maxAge);
    result.push_back(color.x);
    result.push_back(color.y);
    result.push_back(color.z);
  }

  return result;
}

void allocate_soa(soa_point_t *point, size_t size)
{
  point->size = size;
  point->x = reinterpret_cast<float *>(_mm_malloc(sizeof(float)*size, 64));
  assert (point->x != NULL);
  point->y = reinterpret_cast<float *>(_mm_malloc(sizeof(float)*size, 64));
  assert (point->y != NULL);
  point->z = reinterpret_cast<float *>(_mm_malloc(sizeof(float)*size, 64));
  assert (point->z != NULL);
}

void free_soa(soa_point_t *point)
{
  if (point->size > 0) {
    _mm_free(point->x);
    _mm_free(point->y);
    _mm_free(point->z);
  }
  point->size = 0;
}
