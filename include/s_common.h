#ifndef s_common_h_
#define s_common_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <errno.h>

#include <sys/ioctl.h>
#include <unistd.h>

/* -- */
#define MAX_THREAD	256

#define s_offsetof(t, e)	(size_t)(&((t*)0)->e)
#define s_container_of(p, t, e)	(t*)((size_t)(p) - s_offsetof(t, e))

#define s_roundup(n, align)	(((n) + align - 1) & (~(align - 1)))

#define s_max(x, y)	((x) > (y) ? (x) : (y))
#define s_min(x, y)	((x) < (y) ? (x) : (y))

/* - log - */
#define s_log(format, ...)	printf("(%s:%d:%s) "format"\n", __FILE__, __LINE__, __FUNCTION__,  ##__VA_ARGS__)

#endif

