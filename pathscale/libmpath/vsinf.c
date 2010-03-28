#include "vector.h"

vector_declare(sinf);

#ifdef __x86_64__
#include "w_sincosf.c"
#endif

vector_function(sinf, const float *x, float *y);
