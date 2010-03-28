#include "vector.h"

vector_declare(atanf);

#ifdef __x86_64__
#include "s_atanf.c"
#endif

vector_function(atanf, const float *x, float *y);
