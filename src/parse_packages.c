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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "globals.h"
#include "error.h"
#include "parse_packages.h"
#include "utils.h"
#include "vercmp.h"

GHashTable *packages_hash_table;

void packages_free_ht_entry (gpointer key, gpointer package, gpointer cruft);

void
determine_packages_architecture (packages_info *packages)
{

  /* If not passed -A/--architecture, try to auto-detect */
  if (!packages_architecture)
    {
      while (packages->buffer_pos < packages->file_length && !packages_architecture)
	{
	  packages_architecture = get_field (packages->buffer, packages->file_length, packages->buffer_pos, "Architecture: ", "unknown");

	  if (packages_architecture && strstr (packages_architecture, "all") != NULL)
	    {
	      g_free (packages_architecture);
	      packages_architecture = NULL;
	    }

	  if (!packages_architecture)
	    skip_to_next_package (packages->buffer, &packages->buffer_pos, packages->file_length);
	}

      /* Handle cases of a binary-$arch/ tree populated by nothing but Arch: all packages */
      if (!packages_architecture)
	fubar (NONSYS, "Only 'Architecture: all' packages found in Packages file!  Giving up.");

  /* Reset the buffer position to the start */
      packages->buffer_pos = 0;
    }

}

void
process_packages (packages_info *packages)
{

  want_node *want_data;
  Qlist *want_list;
  char *source_version;
  packages_ht_info *old_package;
  int i;

  want_list = NULL;
  want_data = g_malloc (sizeof(*want_data));
  strcpy (want_data->search_string, "Package: ");
  want_data->destination_string = &packages->name;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Version: ");
  want_data->destination_string = &packages->version;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Source: ");
  want_data->destination_string = &packages->source;
  want_list = want_append (want_list, want_data);
  strcpy (want_data->search_string, "Architecture: ");
  want_data->destination_string = &packages->architecture;
  want_list = want_append (want_list, want_data);

  while (packages->buffer_pos < packages->file_length)
    {
      packages->buffer_pos = parse_package (packages->buffer, packages->buffer_pos, packages->file_length, want_list);

      if (!packages->name)
	{
	  debug (debug_packages, "parse_packages: skipping record, no Package: field found.");
	  continue;
	}

      debug (debug_packages, "parse_packages: processing \"%s\"",  packages->name);

      if (packages->architecture)
	{
	  if (strcmp(packages_architecture, packages->architecture) &&
	      strncmp("all", packages->architecture, 4))
	    {
	      debug (debug_packages, "parse_packages: [emily] \"%s\" is neither arch: %s or arch: all, skipping. [%s]", packages->name, packages_architecture, packages->architecture);
	      continue;
	    }
	}
      else
	fubar(NONSYS, "parse_packages: %s has no Architecture field.", packages->name);

      if (packages->source)
	{
	  source_version = strchr (packages->source, '(');
	  if (source_version)
	    {
	      debug (debug_packages, "naima: before [%s] %s", packages->name, packages->version);
	      source_version++;
	      i = 0;
	      while (*(source_version+i) != '\0' && *(source_version+i) != ')')
		i++;
	      g_free (packages->version);
	      packages->version = g_malloc (i+1);
	      strncpy (packages->version, source_version, i);
	      packages->version[i] = '\0';
	      debug (debug_packages, "naima: after [%s] %s", packages->name, packages->version);
	      /* Make packages->source be just the source package name */
	      *(source_version - 2) = '\0';
	    }
	}

      old_package = packages_ht_lookup (packages->name);
      if (old_package)
	{ /* Handle duplicates */
	  i = compare_versions (packages->version, old_package->version, packages->name);
	  debug (debug_packages, "parse_packages: %s is duplicated [%s vs. %s => %d]", packages->name, packages->version, old_package->version, i);
	  if (i < 0)
	    packages_ht_add (packages->name, packages->version, packages->source, packages->architecture);
	}
      else
	packages_ht_add (packages->name, packages->version, packages->source, packages->architecture);

      g_free (packages->version);
      g_free (packages->source);
      g_free (packages->name);

    }

}

/* Functions for the packages architecture's hash table */

void
packages_ht_create(void)
{

  packages_hash_table = g_hash_table_new (quinn_hash_function, quinn_key_compare_function);

  if (packages_hash_table == NULL)
    fubar (SYSERR, "Unable to create hash table for source packages.");

}

void
packages_ht_add (const char *package, const char *version, const char *source, const char *architecture)
{

  char *key;
  packages_ht_info *data;

  debug(debug_packages, "packages_ht_add: %s_%s (%s) to Packages hash table", package, version, source);

  key = xstrdup (package);
  data = g_malloc (sizeof(packages_ht_info));
  data->version = xstrdup(version);
  if (source)
    data->source = xstrdup(source);
  else
    data->source = NULL;
  data->architecture = xstrdup(architecture);

  g_hash_table_insert (packages_hash_table, key, data);

}

packages_ht_info *
packages_ht_lookup (const char *package)
{

  packages_ht_info *data;

  data = g_hash_table_lookup (packages_hash_table, package);

  return (data);

}

void
packages_ht_destroy (void)
{

  g_hash_table_foreach (packages_hash_table, packages_free_ht_entry, NULL);
  g_hash_table_destroy (packages_hash_table);

}

void
packages_free_ht_entry (gpointer key,
			gpointer data,
			gpointer cruft __attribute__ ((unused)))
{

  g_free (key);
  g_free (((packages_ht_info *) data)->source);
  g_free (((packages_ht_info *) data)->version);
  g_free (((packages_ht_info *) data)->architecture);
  g_free (data);

}
