/* Quinn Diff, Determine which packages need recompiled for any given architecture */
/* Copyright (C) 1997-2002, James Troup <james@nocrew.org> */

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
#include <string.h>
#include <glib.h>
#include "arch_specific.h"
#include "error.h"
#include "init.h"
#include "output.h"
#include "parse_sources.h"
#include "parse_packages.h"
#include "utils.h"

/* The following causes "common.h" to produce definitions of all the global
   variables, rather than just "extern" declarations of them. */
#define GLOBAL
#include "globals.h"

int
main (int argc, char **argv)
{

  sources_info *sources;
  packages_info *packages;

  sources = g_malloc (sizeof(*sources));
  sources->name = NULL;
  sources->section = NULL;
  sources->priority = NULL;
  sources->version = NULL;
  sources->binary = NULL;
  sources->architecture = NULL;
  sources->binaries = NULL;
  packages = g_malloc (sizeof(*packages));
  packages->name = NULL;
  packages->version = NULL;
  packages->source = NULL;
  packages->architecture = NULL;

  /* Setup some (global) variables */

  debug_level = 0;
  warn_older = FALSE;
  ignore_arch_all = FALSE;
  program_name = argv[0];
  sources->buffer_pos = 0;
  packages->buffer_pos = 0;
  sources->file_name = xstrdup ("Sources");
  packages->file_name = xstrdup ("Packages");
  packages_arch_specific_filename = xstrdup ("/etc/Packages-arch-specific");
  packages_architecture = NULL;
  parse_opt (argc, argv, sources, packages);

  /* Initalize the hash tables */

  packages_ht_create ();
  arch_specific_packages_ht_create ();
  output_ht_create ();

  /* Read and process the Packages file */

  read_file (packages->file_name, packages->file_pointer, &packages->buffer, &packages->file_length);
  determine_packages_architecture (packages);
  process_packages (packages);
  g_free (packages->buffer);

  /* Read and process Packages-arch-specific */

  arch_specific_packages_read ();

  /* Read and process the Sources file */

  read_file (sources->file_name, sources->file_pointer, &sources->buffer, &sources->file_length);
  parse_sources (sources);

  /* Display the output */

  output_display_all ();

  /* Cleanup */

  packages_ht_destroy ();
  arch_specific_packages_ht_destroy ();
  output_ht_destroy ();
  g_free (sources->buffer);
  g_free (sources);
  g_free (packages);
  g_free (packages_architecture);

  return (0);

}
