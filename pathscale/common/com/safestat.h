/* Bug 15177: Always use 64-bit stat function and struct.  Otherwise,
 * 64-stat data may be discarded during conversion to 32-bit data.
 */

#ifndef TARG_MIPS

#include <sys/stat.h>
#define STAT stat
#define FSTAT fstat

#else  /* TARG_MIPS */

/* Save _LARGEFILE64_SOURCE */
#ifdef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE_SAVED _LARGEFILE64_SOURCE
#endif

#define _LARGEFILE64_SOURCE 1
#define STAT stat64
#define FSTAT fstat64

#include <sys/stat.h>

/* Reset _LARGEFILE64_SOURCE */
#undef _LARGEFILE64_SOURCE
#ifdef _LARGEFILE64_SOURCE_SAVED
#define _LARGEFILE64_SOURCE _LARGEFILE64_SOURCE_SAVED
#endif

#endif  /* TARG_MIPS */
