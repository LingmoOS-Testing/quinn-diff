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

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "parse_sources.h"

typedef struct _output_info output_info;

struct _output_info {
  char *name;
  char *source_version;
  char *binary_version;
  boolean partial;
  boolean missing;
  char *priority;
  char *section;
  int compare_versions_result;
};

void output_ht_create (void);
void output_ht_add (const sources_info *source, const char *binary_version,
		    const int compare_versions_result);
output_info *output_ht_retrieve (const char *package);
void output_display_all(void);
boolean output_ht_remove (const char *key);
void output_ht_destroy(void);

#endif /* __OUTPUT_H__ */
