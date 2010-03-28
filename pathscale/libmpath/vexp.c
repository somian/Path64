#include "vector.h"

vector_declare(exp);

#ifdef __x86_64__
#include "w_exp.c"
#endif

vector_function(exp, const double *x, double *y);
