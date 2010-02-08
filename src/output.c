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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "globals.h"
#include "error.h"
#include "output.h"
#include "parse_sources.h"
#include "utils.h"
#include "vercmp.h"

GHashTable *output_hash_table;

void output_display_package (gpointer key, gpointer xpackage, gpointer cruft);
void output_free_ht_entry (gpointer key, gpointer package, gpointer cruft);

void
output_ht_create(void)
{

  output_hash_table = g_hash_table_new (quinn_hash_function, quinn_key_compare_function);

  if (output_hash_table == NULL)
    fubar (SYSERR, "Unable to create hash table for output buffer.");

}

void
output_ht_add (const sources_info *source, const char *binary_version,
	       const int compare_versions_result)
{

  output_info *package_to_add;
  output_info *old_package;
  char *key;
  int i;

  /* Check for duplicates */

  old_package = output_ht_retrieve (source->name);
  if (old_package)
    {
      i = compare_versions (source->version, old_package->source_version, source->name);
      debug (debug_output, "output_ht_add: %s (%s, %s) is duplicated [%d]",
	     source->name, source->version, old_package->source_version, i);
      if (i > 0)
	{
	  debug (debug_output, "output_ht_add: %s_%s (new) loses to %s",
		 source->name, source->version, old_package->source_version);
	  return;
	}
      else if (i == 0)
	{
	  debug (debug_output, "output_ht_add: %s_%s same version",
		 source->name, source->version);
	  if (binary_version)
	    {
	      /* Update the 'partial' flag */
	      if (!old_package->partial)
		{
		  debug (debug_output, "output_ht_add: duplicate, but this one has binary_version so flipping partial on. [%s]", source->name);
		  old_package->partial = TRUE;
		}
	    }
	  if (!binary_version)
	    {
	      /* Update the 'missing' flag */
	      if (!old_package->missing)
		{
		  debug (debug_output, "output_ht_add: duplicate, but this one has no binary_version so flipping missing on. [%s]", source->name);
		  old_package->missing = TRUE;
		}
	    }
	  if (compare_versions_result != 0)
	    {
	      /* If it was 0 before, whatever we are wins.  If we're <
                 0, we dominate any > 0 entry. */
	      if ((old_package->compare_versions_result == 0) ||
		  (compare_versions_result < 0 &&
		   old_package->compare_versions_result > 0))
		{
		  old_package->compare_versions_result = compare_versions_result;
		  debug (debug_output, "output_ht_add: [angie] %s cvr set to %d",
			 source->name, compare_versions_result);
		}
	      else
		debug (debug_output, "output_ht_add: [angie] %s cvr stays at %d",
		       source->name, compare_versions_result);

	    }
	  return;
	}
      else if (i < 0)
	{
	  debug (debug_output, "output_ht_add: %s_%s (old) loses to %s",
		 source->name, old_package->source_version, source->version);
	  output_ht_remove(source->name);
	  /* Fall through */
	}
    }

  debug(debug_output, "output_ht_add: Adding \"%s\" (%s:%s) [Versions %s and %s] [CVR = %d] to output hash table",
	source->name, source->priority, source->section, source->version,
	binary_version, compare_versions_result);

  /* Set up the output_info node for this package */

  package_to_add = g_malloc (sizeof(output_info));
  package_to_add->name = xstrdup (source->name);
  package_to_add->source_version = xstrdup (source->version);
  if (binary_version)
    {
      package_to_add->binary_version = xstrdup (binary_version);
      package_to_add->partial = TRUE;
      package_to_add->missing = FALSE;
    }
  else
    {
      package_to_add->binary_version = NULL;
      package_to_add->partial = FALSE;
      package_to_add->missing = TRUE;
    }
  package_to_add->priority = xstrdup (source->priority);
  package_to_add->section = xstrdup (source->section);
  package_to_add->compare_versions_result = compare_versions_result;

  key = xstrdup(source->name);

  /* And add it (or rather, a pointer to it) to the hash table */

  g_hash_table_insert (output_hash_table, key, package_to_add);

}

output_info *
output_ht_retrieve (const char *package)
{

  output_info *output_details;

  output_details = g_hash_table_lookup (output_hash_table, package);

  return (output_details);

}

void
output_display_all(void)
{

  g_hash_table_foreach (output_hash_table, output_display_package, NULL);

}

void
output_display_package (gpointer key __attribute__ ((unused)),
			gpointer xpackage,
			gpointer cruft __attribute__ ((unused)))
{

  output_info *package;
  char status[15];

  package = (output_info *) xpackage;

  if (package->compare_versions_result < 0)
    strcpy (status, "out-of-date");
  else if (package->partial)
    strcpy (status, "partial");
  else /* missing */
    strcpy (status, "uncompiled");


  if (package->missing || package->compare_versions_result < 0)
    printf ("%s/%s_%s.dsc [%s:%s]\n",
	    package->section, package->name,
	    package->source_version, package->priority, status);
  else if (warn_older && package->compare_versions_result > 0)
    printf ("OLDER: %s/%s_%s.dsc [%s:%s] (Binary is %s)\n",
	    package->section, package->name, package->source_version,
	    package->priority, status, package->binary_version);

}

boolean
output_ht_remove (const char *key)
{

  output_info *package;

  package = output_ht_retrieve (key);
  if (!package)
    return FALSE;

  g_free (package->name);
  g_free (package->source_version);
  g_free (package->binary_version);
  g_free (package->priority);
  g_free (package->section);
  g_free (package);
  g_hash_table_remove (output_hash_table, key);

  return TRUE;

}


void
output_ht_destroy(void)
{

  g_hash_table_foreach (output_hash_table, output_free_ht_entry, NULL);
  g_hash_table_destroy (output_hash_table);

}

void
output_free_ht_entry (gpointer key __attribute__ ((unused)),
		      gpointer package,
		      gpointer cruft __attribute__ ((unused)))
{

  g_free (key);
  g_free (((output_info *) package)->name);
  g_free (((output_info *) package)->source_version);
  g_free (((output_info *) package)->binary_version);
  g_free (((output_info *) package)->priority);
  g_free (((output_info *) package)->section);
  g_free (package);

}

