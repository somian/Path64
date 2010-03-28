#include "vector.h"

vector_declare(tan);

#ifdef __x86_64__
#include "s_tan.c"
#endif

vector_function(tan, const double *x, double *y);
