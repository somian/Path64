#include "vector.h"

vector_declare(log);

#ifdef __x86_64__
#include "w_log.c"
#endif

vector_function(log, const double *x, double *y);
