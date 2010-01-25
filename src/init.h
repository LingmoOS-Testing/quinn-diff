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

#ifndef __INIT_H__
#define __INIT_H__

#include "parse_sources.h"
#include "parse_packages.h"

void parse_opt (int argc, char **argv, sources_info *sources, packages_info *secondary);
void usage (int status) __attribute__ ((noreturn));

#endif /* __INIT_H__ */
