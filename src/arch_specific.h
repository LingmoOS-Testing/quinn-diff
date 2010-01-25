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

#ifndef __ARCH_SPECIFC_H__
#define __ARCH_SPECIFC_H__

#include "common.h"

void arch_specific_packages_ht_create (void);
void arch_specific_packages_read (void);
boolean is_arch_specific (char *package_name);
void arch_specific_packages_ht_destroy (void);

#endif /* __ARCH_SPECIFC_H__ */
