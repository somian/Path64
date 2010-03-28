#include "vector.h"

vector_declare(expf);

#ifdef __x86_64__
#include "w_expf.c"
#endif

vector_function(expf, const float *x, float *y);
