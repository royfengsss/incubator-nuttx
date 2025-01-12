/****************************************************************************
 * libs/libc/string/lib_bsdstrcmp.c
 *
 * Copyright (c) 1994-2009  Red Hat, Inc. All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the BSD License.   This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
 * including the implied warranties of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  A copy of this license is available at
 * http://www.opensource.org/licenses. Any Red Hat trademarks that are
 * incorporated in the source code or documentation are not subject to
 * the BSD License and may only be used or replicated with the express
 * permission of Red Hat, Inc.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <string.h>

#include "libc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Nonzero if either x or y is not aligned on a "long" boundary. */

#define UNALIGNED(x, y) \
  (((long)(uintptr_t)(x) & (sizeof(long) - 1)) | ((long)(uintptr_t)(y) & (sizeof(long) - 1)))

/* Macros for detecting endchar */

#if LONG_MAX == 2147483647
#  define DETECTNULL(x) (((x) - 0x01010101) & ~(x) & 0x80808080)
#elif LONG_MAX == 9223372036854775807
/* Nonzero if x (a long int) contains a NULL byte. */

#  define DETECTNULL(x) (((x) - 0x0101010101010101) & ~(x) & 0x8080808080808080)
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#if !defined(CONFIG_LIBC_ARCH_STRCMP) && defined(LIBC_BUILD_STRCMP)
#undef strcmp /* See mm/README.txt */
nosanitize_address
int strcmp(FAR const char *cs, FAR const char *ct)
{
  FAR unsigned long *a1;
  FAR unsigned long *a2;

  /* If cs or ct are unaligned, then compare bytes. */

  if (!UNALIGNED(cs, ct))
    {
      /* If cs and ct are word-aligned, compare them a word at a time. */

      a1 = (FAR unsigned long *)cs;
      a2 = (FAR unsigned long *)ct;
      while (*a1 == *a2)
        {
          /* To get here, *a1 == *a2, thus if we find a null in *a1,
           * then the strings must be equal, so return zero.
           */

          if (DETECTNULL(*a1))
            {
              return 0;
            }

          a1++;
          a2++;
        }

      /* A difference was detected in last few bytes of cs,
       * so search bytewise.
       */

      cs = (FAR char *)a1;
      ct = (FAR char *)a2;
    }

  while (*cs != '\0' && *cs == *ct)
    {
      cs++;
      ct++;
    }

  return (*(FAR unsigned char *)cs) - (*(FAR unsigned char *)ct);
}
#endif
