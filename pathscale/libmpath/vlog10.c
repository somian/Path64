#include "vector.h"

vector_declare(log10);

#define COMPILING_LOG10
#ifdef __x86_64__
#include "w_log.c"
#endif

vector_function(log10, const double *x, double *y);
