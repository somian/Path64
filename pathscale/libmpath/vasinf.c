#include "vector.h"

vector_declare(asinf);

#ifdef __x86_64__
#include "w_asinf.c"
#endif

vector_function(asinf, const float *x, float *y);
