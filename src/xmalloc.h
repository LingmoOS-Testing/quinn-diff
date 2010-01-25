/* xmalloc.c -- malloc with out of memory checking
   Copyright (C) 1990, 91, 92, 93, 94, 95, 96 Free Software Foundation, Inc.
   Copyright (C) 1997 James Troup <james@nocrew.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef __XMALLOC_H__
#define __XMALLOC_H__

#include <stdio.h>

void *fixup_null_alloc (size_t n);
void *xmalloc (size_t n);
void *xcalloc (size_t n, size_t s);
void *xrealloc (void *p, size_t n);
#define xfree_if_non_null(p) if (p) { free (p); p = NULL; };
#define xfree(p) free (p); p=NULL;

#endif /* __XMALLOC_H__ */
