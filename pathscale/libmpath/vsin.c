#include "vector.h"

vector_declare(sin);

#ifdef __x86_64__
#include "w_sincos.c"
#endif

vector_function(sin, const double *x, double *y);
