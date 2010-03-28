#include "vector.h"

vector_declare(cos);

#ifdef __x86_64__
#include "w_sincos.c"
#endif

vector_function(cos, const double *x, double *y);
