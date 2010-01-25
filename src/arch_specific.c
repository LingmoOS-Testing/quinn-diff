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
#include "utils.h"
#include "xmalloc.h"
#include "arch_specific.h"

/* Used to stop quinn diff trying to parse Packages-arch-specific
   files which it can't */

#define NEWEST_PARSEABLE_VERSION 3

GHashTable *arch_specific_packages_hash_table;

void arch_specific_package_free_ht_entry (gpointer key, gpointer package, gpointer cruft);
void open_arch_specific_file(FILE *fp, char **buffer_p, long *buffer_length);
void read_arch_specific_packages (const char *buffer_p, const unsigned long buffer_length);
void arch_specific_packages_ht_add (char *package_name);
void check_parseable (const char *buffer_p, const unsigned long buffer_length);

/* Public functions */

void
arch_specific_packages_ht_create (void)
{

  arch_specific_packages_hash_table = g_hash_table_new (quinn_hash_function, quinn_key_compare_function);

  if (arch_specific_packages_hash_table == NULL)
    fubar (SYSERR, "Unable to create hash table for architecture specific packages.");

}

void
arch_specific_packages_read (void)
{

  FILE *fp = NULL;
  char *buffer_p;
  unsigned long buffer_length;

  open_arch_specific_file (fp, &buffer_p, &buffer_length);
  debug (debug_as, "read_arch_specific_file: read %ld bytes of \"%s\"",
	buffer_length, packages_arch_specific_filename);

  read_arch_specific_packages (buffer_p, buffer_length);
  xfree (buffer_p);

}

boolean
is_arch_specific (char *package)
{

  gpointer arch_specific;

  arch_specific = g_hash_table_lookup (arch_specific_packages_hash_table, package);

  if (arch_specific)
    {
      debug (debug_as, "is_arch_specific: %s is.", package);
      return TRUE;
    }
  else
    return FALSE;

}

void
arch_specific_packages_ht_destroy (void)
{

  g_hash_table_foreach (arch_specific_packages_hash_table, arch_specific_package_free_ht_entry, NULL);
  g_hash_table_destroy (arch_specific_packages_hash_table);

}

void
arch_specific_package_free_ht_entry (gpointer key,
				     gpointer package __attribute__ ((unused)),
				     gpointer cruft __attribute__ ((unused)))
{

  xfree (key);

}

/* Private functions */

void open_arch_specific_file(FILE *fp, char **buffer_p, long *buffer_length)
{
  long n_read;

  fp = fopen (packages_arch_specific_filename, "r");
  if (fp == NULL)
    fubar (SYSERR, "%s: couldn't open file \"%s\"",
	   program_name, packages_arch_specific_filename);
  if ((fseek (fp, 0, SEEK_END)) != 0)
    fubar (SYSERR, "%s: couldn't seek to the end of file \"%s\"",
	   program_name, packages_arch_specific_filename);
  *buffer_length = ftell (fp);
  rewind (fp);

  /* FIXME: this probably shouldn't bomb out */

  if (*buffer_length == 0)
    fubar (NONSYS, "%s: \"%s\" is zero-length.", program_name, packages_arch_specific_filename);

  /* We don't do any memory management, we just ask for a chunk of
   * memory the size of the file */

  *buffer_p = xmalloc (*buffer_length);

  n_read = fread (*buffer_p, sizeof (char), *buffer_length, fp);
  if (n_read == 0)
    fubar (SYSERR, "%s: couldn't read file \"%s\" into buffer",
	   program_name, packages_arch_specific_filename);
  else if (n_read < *buffer_length)
    fubar (SYSERR, "%s: short read (found %ld, expected %ld) in file \"%s\"",
	   program_name, n_read, *buffer_length, packages_arch_specific_filename);

  if (fclose (fp) == EOF)
    fubar (SYSERR, "%s: couldn't close file \"%s\"", program_name,
	   packages_arch_specific_filename);
}


void read_arch_specific_packages (const char *buffer_p, const unsigned long buffer_length)
{

  char *line;
  char *architectures;
  char *package_name;
  unsigned long i;
  unsigned int j, k, l;

  check_parseable (buffer_p, buffer_length);

  i = 0;

  while (i < buffer_length)
    {

      /* We don't need to bother with either comment lines or empty lines */

      if (buffer_p[i] == ' ' || buffer_p[i] == '#')
	{
	  skip_line (buffer_p, &i, buffer_length);
	  continue;
	}

      line = read_line (buffer_p, buffer_length, &i);

      /* Parse the line by splitting it up into package name and the packages' supported architectures */

      /* Package name is everything up to the first `:' */

      j = 0;
      k = 0;
      while (line[j] != ':' && j < strlen(line))
	j++;
      package_name = xmalloc (j + 1);
      for (k = 0, l = 0; k < j; k++, l++)
	package_name[k] = line[l];
      package_name[k] ='\0';
      debug(debug_as, "package name is \"%s\"", package_name);

      /* Architectures is everything after that (excluding comments) */

      k = 0;
      j = strlen (line) - l;
      architectures = xmalloc (j + 1);
      while (k < j && line[l] != '#')
	if (line[l] == '\t')
	  {
	    architectures[k++] = ' ';
	    l++;
	  }
	else
	    architectures[k++] = line[l++];
      architectures[k] = '\0';
      debug(debug_as, "architectures are \"%s\"", architectures);

      /* Check for "!arch" or "arch" */

      if (strchr (architectures, '!') != NULL)
	{
	  /* "!arch" is exclusionary; if there's a !something, but
	     not !arch there is support for arch */
	  char *w;
	  char *p;

	  for (w = p = architectures ; *w ; w++ )
	    if (*w != '!')
	      *p++ = *w;
	  *p = '\0';

	  if (in_arch_list(architectures, packages_architecture))
	    {
	      debug (debug_as, "read_arch_specific_packages: %s doesn't need compiled on %s [exc]",
		     package_name, packages_architecture);
	      arch_specific_packages_ht_add(package_name);
	    }
	}
      else
	{
	  /* "arch" is inclusionary; if arch is not listed, it's not
	     supported by the package */
	  if (in_arch_list (architectures, packages_architecture))
	    debug (debug_as, "read_arch_specficic_packages: %s has %s support",
		   package_name, packages_architecture);
	  else
	    {
	      debug (debug_as, "read_arch_specficic_packages: %s doesn't need compiled on %s [inc]",
		     package_name, packages_architecture);
	      arch_specific_packages_ht_add(package_name);
	    }
	}

      xfree (line);
      xfree (package_name);
      xfree (architectures);

    }

}

void arch_specific_packages_ht_add (char *package)
{

  char *key;

  key = xstrdup (package);

  debug (debug_as, "arch_specific_hash_add: Adding \"%s\" to arch specific hash table", package);

  /* NB: The 1 is a dummy number; it doesn't matter what it is, as
     long as it isn't 0 */

  g_hash_table_insert (arch_specific_packages_hash_table, key, (gpointer) 1);


}

  /* First line should be a special comment field giving the version
     number of the Packages-arch-specific file; use this to ensure
     we're not trying to parse something we don't understand */

void check_parseable (const char *buffer_p, const unsigned long buffer_length)
{

  char *line;
  char *arch_specific_version_string, *errors;
  unsigned long i;
  long arch_specific_version;

  i = 0;

  if (buffer_p[i] == '#')
    {
      line = read_line (buffer_p, buffer_length, &i);
      arch_specific_version_string = strstr (line, "Packages-arch-specific-version: ");
      if (arch_specific_version_string != NULL)
	{
	  arch_specific_version_string += 32;
	  arch_specific_version = strtol(arch_specific_version_string, &errors, 10);
	  if (errors != arch_specific_version_string)
	    {
	      if (arch_specific_version > NEWEST_PARSEABLE_VERSION)
		fubar (NONSYS, "I can't understand this type of Packages-arch-specific file; please upgrade.");
	    }
	  else
	    fubar (NONSYS, "I can't understand this type of Packages-arch-specific file; please upgrade.");
	}
      xfree (line);
    }

}
