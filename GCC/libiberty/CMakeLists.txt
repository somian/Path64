include_directories(BEFORE SYSTEM .)

set(IBERTY_SRCS alloca.c
		argv.c
		choose-temp.c
		concat.c
		cp-demangle.c
		cplus-dem.c
		dyn-string.c
		fdmatch.c
		fibheap.c
		floatformat.c
		fnmatch.c
		getopt.c
		getopt1.c
		getpwd.c
		getruntime.c
		hashtab.c
		hex.c
		lbasename.c
		make-temp-file.c
		md5.c
		mkstemps.c
		objalloc.c
		obstack.c
		partition.c
		pexecute.c
		physmem.c
		safe-ctype.c
		spaces.c
		splay-tree.c
		strerror.c
		strsignal.c
		xatexit.c
		xexit.c
		xmalloc.c
		xmemdup.c
		xstrdup.c
		xstrerror.c)

add_library(iberty STATIC ${IBERTY_SRCS})
