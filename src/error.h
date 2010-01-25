/* Quinn Diff, Determine which packages need recompiled for any given architecture */
/* Copyright (C) 1997-1999, James Troup <james@nocrew.org> */

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

#ifndef __ERROR_H__
#define __ERROR_H__

void debug (const int message_level, const char *fmt,...) __attribute__ ((format (printf, 2, 3)));
void debug_help (void);
void error (const char *fmt,...) __attribute__ ((format (printf, 1, 2)));
void fubar (unsigned short flag, const char *fmt,...) __attribute__((noreturn,format(printf, 2, 3)));

/* Flags for debug */

#define debug_warn      0001  /* General warnings, supersedes --warn-* */
#define debug_sources   0002  /* Sources file */
#define debug_packages  0004  /* Packages file */
#define debug_as        0010  /* Architecture Specific packages */
#define debug_output    0020  /* Output functions */
#define debug_versions  0040  /* Version comparsion functions */
#define debug_utils     0100  /* Util functions */

/* Flags for fubar */

#define NONSYS 0x0001
#define SYSERR 0x0002

#endif /* __ERROR_H__ */
