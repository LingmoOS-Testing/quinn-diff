/* Quinn Diff, Determine which packages need recompiled for any given architecture */
/* Copyright (C) 1997-2002 James Troup <james@nocrew.org> */

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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "arch_specific.h"
#include "error.h"
#include "globals.h"
#include "list.h"
#include "output.h"
#include "parse_packages.h"
#include "parse_sources.h"
#include "utils.h"
#include "vercmp.h"

void
parse_sources (sources_info *source)
{

  want_node *want_data;
  Qlist *want_list, *list_temp;
  char *binary, *prefixed_source_name;
  packages_ht_info *binary_data;
  int i, q;
  boolean is_different;

  /* Read the Sources file, comparing to the the hash table of the
     Packages file as we go. */

  want_list = NULL;
  want_data = g_malloc (sizeof(*want_data));
  strcpy (want_data->search_string, "Package: ");
  want_data->destination_string = &source->name;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Priority: ");
  want_data->destination_string = &source->priority;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Section: ");
  want_data->destination_string = &source->section;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Version: ");
  want_data->destination_string = &source->version;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Binary: ");
  want_data->destination_string = &source->binary;
  want_list = want_append (want_list, want_data);
  /* NB: the search string must _not_ include the space so we can do sane searching. */
  strcpy (want_data->search_string, "Architecture:");
  want_data->destination_string = &source->architecture;
  want_list = want_append (want_list, want_data);

  while (source->buffer_pos < source->file_length)
    {

      source->buffer_pos = parse_package (source->buffer, source->buffer_pos, source->file_length, want_list);

      /* Check the entire source package is not excluded */

      prefixed_source_name = g_malloc (strlen (source->name) + 2);
      sprintf (prefixed_source_name, "%%%s", source->name);
      q = is_arch_specific (prefixed_source_name);
      g_free (prefixed_source_name);
      if (q) {
        g_free (source->name);
        source->name = NULL;
        continue;
      }

      /* If we're ignoring 'Architecture: all' packages, check this isn't */
      if (ignore_arch_all && in_arch_list(source->architecture, "all")
	  && !in_arch_list(source->architecture, packages_architecture))
	{
	  debug (debug_sources, "parse_sources: ignoring 'Architecture: all' source packages, and %s is ('%s').", source->name, source->architecture);
	  continue;
	}

      /* Sanity check source package's fields before continuing */

      if (!source->section)
	source->section = xstrdup ("");

      if (!source->priority)
	source->priority = xstrdup ("-");

      list_temp = want_find (want_list, "Version: ");
      if (!((want_node *)(list_temp->data))->found)
	fubar (NONSYS, "Source package \"%s\" has no version field.", source->name);

      debug(debug_sources, "parse_sources: priority = %s, section = %s, version = %s, binary = %s",
	    source->priority, source->section, source->version, source->binary);

      /* Process each binary */

      source->binaries = g_strsplit(source->binary, ",", 0);
      for(i = 0; source->binaries[i]; ++i)
      {
        binary = g_strstrip(source->binaries[i]);
        debug (debug_sources, "parse_sources: source %s: processing binary %s", source->name, binary);

	  if (!is_arch_specific(binary))
	    {
	      binary_data = packages_ht_lookup (binary);
	      is_different = FALSE;
	      /* If this binary package is not compiled... */
	      if (!binary_data)
		{
		  debug (debug_sources, "parse_sources: %s from %s is uncompiled...", binary, source->name);
		  output_ht_add (source, NULL, 0);
		  is_different = TRUE;
		}
	      /* If it is compiled, make sure it's the same version... */
	      else
		{
		  if (strcmp (source->version, binary_data->version))
		    {
		      q = compare_versions (source->version, binary_data->version, source->name);
		      /* If the binary is newer than the source, ensure it's a binary
                         from this source package */
		      if ((q >= 1) &&
			  (binary_data->source && (strcmp (binary_data->source, source->name))))
			debug(debug_sources, "parse_sources: %s is from %s, not %s and  is a newer version (%s vs. %s) too.", binary, binary_data->source, source->name, binary_data->version, source->version);
		      else
			{
			  debug (debug_sources, "parse_sources: %s from %s is different version-wise (%d)...", binary, source->name, q);
			  output_ht_add (source, binary_data->version, q);
			  is_different = TRUE;
			}
		    }
		  else
		    {
		      if (strcmp(binary_data->architecture, "all"))
			output_ht_add (source, binary_data->version, 0);
		    }
		}

	      if (is_different &&
		  !in_arch_list(source->architecture,packages_architecture) &&
		  !in_arch_list(source->architecture,"any"))
		error ("warning: %s has an architecture field of \"%s\" which doesn't include %s.", source->name, source->architecture+1, packages_architecture);
	    }
	}

      g_strfreev(source->binaries);

    }

  source->binaries = NULL;
  qlist_destroy (want_list);

}

