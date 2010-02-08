#ifndef __COMMON_H__
#define __COMMON_H__

#include "config.h"

#ifndef SUPPORT__BUILTIN_EXPECT
# define __builtin_expect(stmt, flag) (stmt)
#endif

#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

typedef int boolean;

#ifndef FALSE
#define FALSE (0)
#endif /* FALSE */

#ifndef TRUE
#define TRUE (!FALSE)
#endif /* TRUE */

#endif /* __COMMON_H__ */
