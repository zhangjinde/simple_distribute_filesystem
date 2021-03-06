#ifndef s_common_h_
#define s_common_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <errno.h>

#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_THREAD	256

struct s_id {
	int x;
	int y;
};

#define s_id_gt(a, b)	( ( (a)->y > (b)->y ) || ( ( (a)->y == (b)->y ) && (a)->x > (b)->x ) )

/* - bit - */
#define S_SETBIT(f, b)		f |= (b)
#define S_CLEARBIT(f, b)	f &= ~(b)

/* - util - */
#define s_used(x)	(void)(x)

#define s_offsetof(t, e)	(size_t)(&((t*)0)->e)
#define s_container_of(p, t, e)	(t*)((size_t)(p) - s_offsetof(t, e))

#define s_roundup(n, align)	(((n) + align - 1) & (~(align - 1)))

#define s_max(x, y)	((x) > (y) ? (x) : (y))
#define s_min(x, y)	((x) < (y) ? (x) : (y))

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

/* - log - */
#ifdef S_DEBUG_LOG
#define s_log(format, ...)	printf("(%s:%d:%s) "format"\n", __FILE__, __LINE__, __FUNCTION__,  ##__VA_ARGS__)
#else
#define s_log(format, ...)	printf(format"\n",  ##__VA_ARGS__)
#endif

#endif

