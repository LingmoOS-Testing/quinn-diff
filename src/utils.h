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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <glib.h>
#include "list.h"

guint quinn_hash_function (gconstpointer keyarg);
gint quinn_key_compare_function (gconstpointer a, gconstpointer b);
long parse_package (const char *buffer, long buffer_position, const int buffer_length, 
		    Qlist *want_list);
char *get_field (const char *buffer, const long buffer_length, long buffer_position, 
		const char *input, const char *package_name);
void skip_line (const char *buffer, long *counter, const long buffer_length);
void skip_to_next_package (const char *buffer, long *buffer_position, const long buffer_length);
char *read_line (const char *buffer, const long buffer_length, long *counter);
void read_file (const char *filename, FILE *fp, char **buffer_p, long *file_length);
char *xstrdup (const char *s);
#define strmove(dest, src) memmove (dest, src, strlen (src) + 1)
boolean in_arch_list(const char *list, const char *arch);

#endif /* __UTILS_H__ */
