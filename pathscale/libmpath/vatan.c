#include "vector.h"

vector_declare(atan);

#ifdef __x86_64__
#include "s_atan.c"
#endif

vector_function(atan, const double *x, double *y);
