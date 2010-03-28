#include "vector.h"

vector_declare(acosf);

#ifdef __x86_64__
#include "w_acosf.c"
#endif

vector_function(acosf, const float *x, float *y);
