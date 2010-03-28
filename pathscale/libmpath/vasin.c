#include "vector.h"

vector_declare(asin);

#ifdef __x86_64__
#include "w_asin.c"
#endif

vector_function(asin, const double *x, double *y);
