#ifndef __SOA_POINT_H
#define __SOA_POINT_H

//#include <cstdlib>

typedef struct
{
  float *x;
  float *y;
  float *z;
  unsigned int size;
} soa_point_t;

#endif
