#ifndef __SOA_POINT_H
#define __SOA_POINT_H

// designed to make it a bit easier to pass around SOA
// data without declaring a ton of float pointers

typedef struct
{
  float *x;
  float *y;
  float *z;
  unsigned int size;
} soa_point_t;

#endif
