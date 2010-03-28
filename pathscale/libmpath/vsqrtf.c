#include "vector.h"

vector_declare(sqrtf);

#ifdef __x86_64__
#define USE_NANF_WITH_FLAGS
#define USE_SQRTF_AMD_INLINE
#define sqrtf_amd_inline FN_NAME(sqrtf)
#include "libm_inlines_amd.h"
#endif

vector_function(sqrtf, const float *x, float *y);
