#include "vector.h"

vector_declare(sqrt);

#ifdef __x86_64__
#define USE_NAN_WITH_FLAGS
#define USE_SQRT_AMD_INLINE
#define sqrt_amd_inline FN_NAME(sqrt)
#include "libm_inlines_amd.h"
#endif

vector_function(sqrt, const double *x, double *y);
