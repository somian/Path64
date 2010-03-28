#include "vector.h"

vector_declare(log10f);

#define COMPILING_LOG10
#ifdef __x86_64__
#include "w_logf.c"
#endif

vector_function(log10f, const float *x, float *y);
