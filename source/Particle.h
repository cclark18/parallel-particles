#ifndef __PARTICLE_H
#define __PARTICLE_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  float age;
  glm::vec3 position;
} Particle;

#endif
