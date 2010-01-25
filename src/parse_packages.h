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

#ifndef __PARSE_PACKAGES_H__
#define __PARSE_PACKAGES_H__

typedef struct _packages_info packages_info;
typedef struct _packages_ht_info packages_ht_info;

struct _packages_info {
  char *file_name;
  FILE *file_pointer;
  long file_length;
  char *buffer;
  long buffer_pos;
  char *name;
  char *version;
  char *source;
  char *architecture;
};

struct _packages_ht_info {
  char *version;
  char *source;
  char *architecture;
};

void determine_packages_architecture (packages_info *packages);
void process_packages (packages_info *packages);
void packages_ht_create (void);
void packages_ht_add (const char *package, const char *version, const char *source, const char *architecture);
packages_ht_info *packages_ht_lookup (const char *package);
void packages_ht_destroy(void);

#endif /* __PARSE_PACKAGES_H__ */
