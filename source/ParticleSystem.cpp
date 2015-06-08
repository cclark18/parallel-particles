#include "ParticleSystem.h"
//#define DEBUG

ParticleSystem::ParticleSystem()
{
  baseColor = glm::vec3(0.0f, 0.5f, 0.9f);
  allocate_soa(&particlePositionsOld, partIncr);
  allocate_soa(&particlePositionsNew, partIncr);
  meshPositions.size = 0;
  srand(time(NULL));
}

ParticleSystem::~ParticleSystem()
{
  if (particlePositionsOld.size > 0) {
    free_soa(&particlePositionsOld);
  }
  if (particlePositionsNew.size > 0) {
    free_soa(&particlePositionsNew);
  }
  if (meshPositions.size > 0) {
    free_soa(&meshPositions);
  }
}

void ParticleSystem::update(float step)
{
  // update time values, discard particles that have aged completely
  auto iter = particles.begin();
  while (iter != particles.end()) {
    iter->age += step;
    if (iter->age > maxAge) {
      std::cout << "disposing particle... total: " << particles.size() << std::endl;
      iter = particles.erase(iter);
    }
    else {
      ++iter;
    }
  }

  // move particles
  getSOAPositions(&particlePositionsOld);
  assert(particlePositionsOld.size == particlePositionsNew.size);
  calculate(particles.size(), particlePositionsOld, meshPositions, particlePositionsNew, false, step);
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

  if (particlePositionsOld.size <= particles.size()) {
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
  if (size > 0) {
    free_soa(&meshPositions);
  }

  for (size_t s = 0; s < mesh->shapes.size(); ++s) {
    size += mesh->shapes[s].mesh.positions.size() / 3;
  }

  allocate_soa(&meshPositions, size);
  std::cout << size << std::endl;
  getSOAMeshes(&meshPositions);
}

void ParticleSystem::getSOAPositions(soa_point_t *result)
{
  for (int i = 0; i < particles.size(); ++i) {
    result->x[i] = particles[i].position.x;
    result->y[i] = particles[i].position.y;
    result->z[i] = particles[i].position.z;
  }
}


void ParticleSystem::setSOAPositions(soa_point_t values)
{
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
}

// this is where the magic (parallelism) happens
void calculate(size_t numParts,
               soa_point_t positions,
               soa_point_t meshPoints,
               soa_point_t out,
               bool offload,
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

#pragma omp parallel for simd
#else
#pragma omp parallel for
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
      float force = 1.0 * step * FORCE_CONSTANT / (distTot * distTot * distTot);
      dx += distx * force;
      dy += disty * force;
      dz += distz * force;
    }

#ifdef DEBUG
    std::cout << "----- MESH POSITIONS -----" << std::endl;
    std::cout << meshPoints.size << std::endl;
#endif
    // move away from mesh points
    for (size_t j = 0; j < meshPoints.size; ++j) {
      /*float distx = meshPosX[j] - partPosX[i];
      float disty = meshPosY[j] - partPosY[i];
      float distz = meshPosZ[j] - partPosZ[i];*/
      float distx = partPosX[i] - meshPosX[j];
      float disty = partPosY[i] - meshPosY[j];
      float distz = partPosZ[i] - meshPosZ[j];
      float distTot = sqrt(distx * distx + disty * disty + distz * distz + 0.000000000001);  // offset to avoid problems dividing by zero
      float force = 1.0 * step * FORCE_CONSTANT / (distTot * distTot * distTot);
      dx += distx * force;
      dy += disty * force;
      dz += distz * force;
#ifdef DEBUG
      std::cout << "<" << meshPosX[j] << ", " << meshPosY[j] << ", " << meshPosZ[j] << ">" << std::endl;
#endif
    }

    dy -= 0.01;
    // apply translations
    /*outX[i] = partPosX[i];
    outY[i] = partPosY[i];
    outZ[i] = partPosZ[i];*/
    outX[i] = partPosX[i] + dx;
    outY[i] = partPosY[i] + dy;
    outZ[i] = partPosZ[i] + dz;
  }
}

std::vector<float> ParticleSystem::getPositions()
{
  std::vector<float> result;

  for (auto iter = particles.begin(); iter != particles.end(); ++iter) {
    result.push_back(iter->position.x);
    result.push_back(iter->position.y);
    result.push_back(iter->position.z);
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
  //point->x = (float *)(malloc(sizeof(float)*size));
  point->x = reinterpret_cast<float *>(_mm_malloc(sizeof(float)*size, 64));
  assert (point->x != NULL);
  //point->y = (float *)(malloc(sizeof(float)*size));
  point->y = reinterpret_cast<float *>(_mm_malloc(sizeof(float)*size, 64));
  assert (point->y != NULL);
  //point->z = (float *)(malloc(sizeof(float)*size));
  point->z = reinterpret_cast<float *>(_mm_malloc(sizeof(float)*size, 64));
  assert (point->z != NULL);
}

void free_soa(soa_point_t *point)
{
  _mm_free(point->x);
  _mm_free(point->y);
  _mm_free(point->z);
  point->size = 0;
}
