/* Quinn Diff, Determine which packages need recompiled for any given architecture */
/* Copyright (C) 1997-1999 James Troup <james@nocrew.org> */

/* This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef VERCMP_H
#define VERCMP_H

int compare_versions (const char *source_version_string,
		      const char *binary_version_string, 
		      const char *package);

#endif /* VERCMP_H */
