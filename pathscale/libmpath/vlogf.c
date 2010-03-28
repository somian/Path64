#include "vector.h"

vector_declare(logf);

#ifdef __x86_64__
#include "w_logf.c"
#endif

vector_function(logf, const float *x, float *y);
