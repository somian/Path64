#include "vector.h"

vector_declare(cosf);

#ifdef __x86_64__
#include "w_sincosf.c"
#endif

vector_function(cosf, const float *x, float *y);
