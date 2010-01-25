/* Quinn Diff, Determine which packages need recompiled for any given architecture */
/* Copyright (C) 1997-2001, James Troup <james@nocrew.org> */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "globals.h"
#include "error.h"

void
debug (const int which, const char *fmt,...)
{

  va_list args;

  if (!(which & debug_level))
    return;

  fprintf (stderr, "D0%05o: ", which);
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
  putc ('\n', stderr);
}

void
debug_help (void)
{

  printf ("Quinn Diff debugging option, --debug=<octal value> or -D<octal value>:\n\
  number  description\n\
       1  General warnings\n\
       2  Debug information for Sources\n\
       4  Debug information for Packages\n\
      10  Debug information for Architecture Specific packages\n\
      20  Debug information for output functions\n\
      40  Debug information for version comparison functions\n\
     100  Debug information for utility functions\n\
\n\
Combine debug options with bitwise or.\n\
Meanings and values subject to change at any time.\n");
  exit (0);
}

void
error (const char *fmt,...)
{
  va_list args;

  fprintf (stderr, "[%s]: ", program_name);
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
  putc ('\n', stderr);
}

void
fubar (unsigned short flag, const char *fmt,...)
{

  unsigned short syserr = flag & SYSERR;
  va_list args;

  fprintf (stderr, "[%s]: ", program_name);
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
  if (syserr)
    fprintf (stderr, " : %s", strerror (errno));
  putc ('\n', stderr);

  exit (1);
}
