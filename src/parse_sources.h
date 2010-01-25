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

#ifndef __PARSE_SOURCES_H__
#define __PARSE_SOURCES_H__

#include <stdio.h>
#include "list.h"

typedef struct _sources_info sources_info;

struct _sources_info {
  char *file_name;
  FILE *file_pointer;
  long file_length;
  char *buffer;
  long buffer_pos;
  char *name; 
  char *priority;
  char *section;
  char *version;
  char *binary;
  char *architecture;
  Qlist *binaries;
};

void parse_sources (sources_info *sources);

#endif /* __PARSE_SOURCES_H__ */
