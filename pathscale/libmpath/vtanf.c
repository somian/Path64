#include "vector.h"

vector_declare(tanf);

#ifdef __x86_64__
#include "s_tanf.c"
#endif

vector_function(tanf, const float *x, float *y);
