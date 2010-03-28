#include "vector.h"

vector_declare(acos);

#ifdef __x86_64__
#include "w_acos.c"
#endif

vector_function(acos, const double *x, double *y);
