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
#include "common.h"
#include "error.h"
#include "list.h"
#include "utils.h"
#include "xmalloc.h"

/* Two common functions used by all the hash tables */

/*
 * Phong's linear congruential hash
 */
#define dcharhash(h, c)	((h) = 0x63c63cd9*(h) + 0x9c39c33d + (c))

guint
quinn_hash_function (gconstpointer keyarg)
{

  const unsigned char *e, *key;
  unsigned int h;
  unsigned char c;
  unsigned int len;

  len = strlen (keyarg);
  key = keyarg;
  e = key + len;
  for (h = 0; key != e;)
    {
      c = *key++;
      if (!c && key > e)
	break;
      dcharhash(h, c);
    }
  return (h);

}

gint
quinn_key_compare_function (gconstpointer a, gconstpointer b)
{

  return (!strcmp(a, b));

}

/* Non-destructively searches one packages' entry for interesting (as
   defined by the want list) information.*/

long
parse_package (const char *buffer, long buffer_position, const int buffer_length,
	       Qlist *want_list)
{

  char *line;
  Qlist *want_list_pointer;
  int i;

  /* Initialize the want list */
  want_list_pointer = want_list;

  while (want_list_pointer)
    {
      ((want_node *)(want_list_pointer->data))->found = FALSE;
      if (*((want_node *) want_list_pointer->data)->destination_string)
	{
	  xfree (*((want_node *) want_list_pointer->data)->destination_string);
	  *((want_node *) want_list_pointer->data)->destination_string = NULL;
	}
      want_list_pointer = want_list_pointer->next;
    }

  debug (debug_utils, "parse_package: inside parse_packages...");

  while (buffer[buffer_position] != '\n'  && buffer_position < buffer_length)
    {
      want_list_pointer = want_list;

      /* We're not interested in descriptions, so we skip any lines that start with a space. */
      if (buffer[buffer_position] == ' ')
	{
	  skip_line(buffer, &buffer_position, buffer_length);
	  continue;
	}

      i = 0;

      line = read_line (buffer, buffer_length, &buffer_position);

      debug (debug_utils, "parse_package: |->'%s'", line);
      /* Check the want list, to see if the line we have is one we need. */
      /* This code is horrible, sorry */
      while (want_list_pointer)
	{
	  /* Skip wants we already have */
	  if (!((want_node *)(want_list_pointer->data))->found)
	    {
	      if (!strncasecmp (line,
				((want_node *)(want_list_pointer->data))->search_string,
				strlen (((want_node *)(want_list_pointer->data))->search_string)))
		{
		  debug (debug_utils, "parse_package: match on  = %s", (((want_node *)(want_list_pointer->data))->search_string));
		  *((want_node *) want_list_pointer->data)->destination_string =
		    xstrdup (line + (strlen (((want_node *)(want_list_pointer->data))->search_string)));
		  ((want_node *)(want_list_pointer->data))->found = TRUE;
		  break;
		}
	    }

	  want_list_pointer = want_list_pointer->next;
	}

      xfree (line);
    }

  buffer_position++;

  return buffer_position;

}

/* A less heavy weight version of the above; retained because we only
   care about one or two fields in the secondary architecture */

char *
get_field (const char *buffer, const long buffer_length, long buffer_position,
	   const char *input, const char *package_name)
{

  char *field = NULL;

  debug (debug_utils, "get_field: searching for %s in %s", input, package_name);

  while (buffer_position < buffer_length)
    {
      /* If we encounter a blank line, we've overun the package description; give up */
      if (buffer[buffer_position] == '\n')
	{
	  break;
	}

      if (buffer[buffer_position] == input[0])
	{
	  field = read_line (buffer, buffer_length, &buffer_position);

	  /* If we have a match stop searching, if not null the field string */
	  if (!(strncasecmp (input, field, strlen(input))))
	    {
	      strmove (field, field + (strlen (input)));
	      break;
	    }
	  else
	    {
	      xfree (field);
	      field = NULL;
	    }
	}
      else
	skip_line(buffer, &buffer_position, buffer_length);
    }

  return field;

}

void skip_line (const char *buffer, long *counter, const long buffer_length)
{

  while (*counter < buffer_length && buffer[*counter] != '\n')
    (*counter)++;

  if (*counter < buffer_length)
    (*counter)++;          /* Skip the newline */

}

void skip_to_next_package (const char *buffer, long *buffer_position, const long buffer_length)
{

  while (*buffer_position < buffer_length && buffer[*buffer_position] !='\n')
    skip_line (buffer, buffer_position, buffer_length);

  if (*buffer_position < buffer_length)
    (*buffer_position)++;

}

char *
read_line (const char *buffer, const long buffer_length, long *counter)
{

  /* Reads in a line, allocating space on the fly */

  int i = 0;
  int max_length = 256;
  char *line;

  line = xmalloc (max_length);

  while (*counter < buffer_length && buffer[*counter] != '\n')
    {
      if (i == max_length - 1)
	{
	  max_length *= 2;
	  line = xrealloc (line, max_length);
	}
      line[i++] = buffer[(*counter)++];

    }

  (*counter)++;           /* Skip the newline */
  line[i] = '\0';       /* Terminate the string */

  return line;

}

void read_file (const char *filename, FILE *fp, char **buffer_p, long *file_length)
{

  int n_read;

  fp = fopen (filename, "r");
  if (fp == NULL)
    fubar (SYSERR, "read_file: couldn't open file \"%s\"", filename);
  if ((fseek (fp, 0, SEEK_END)) != 0)
    fubar (SYSERR, "read_file: couldn't seek to the end of file \"%s\"", filename);
  *file_length = ftell (fp);
  rewind (fp);

  if (*file_length == 0)
    {
      error ("read_file: \"%s\" is zero-length; hope you know what you're doing...", filename);
      *buffer_p = NULL;
    }
  else
    {
      /* We don't do any memory management, we just ask for a chunk of
       * memory the size of the file */

      *buffer_p = (char *) xmalloc (*file_length);

      /* RUARI QUINN this is ignoring the possibility that ftell could be long */
      n_read = fread (*buffer_p, sizeof (char), *file_length, fp);
      if (n_read == 0)
	fubar (SYSERR, "read_file: couldn't read file \"%s\" into buffer", filename);
      else if (n_read < *file_length)
	fubar (NONSYS, "read_file: short read (found %d, expected %ld) in file \"%s\"",
	       n_read, *file_length, filename);
    }

  if (fclose (fp) == EOF)
    fubar (SYSERR, "read_file: couldn't close file \"%s\"", filename);

}

char *
xstrdup (const char *s)
{
  char *r;

  r = strdup (s);
  if (!r)
    fubar (SYSERR, "xstrdup: memory exhausted.");
  return r;
}

boolean
in_arch_list(const char *arch_list, const char *arch)
{
  char **archs;
  int i;

  if ((archs = g_strsplit(arch_list," ",0)) != NULL)
    {
      for ( i = 0 ; archs[i] ; i++ )
	{
	  if (!strcmp(archs[i],arch))
	    {
	      g_strfreev(archs);
	      return TRUE;
	    }
	}
      g_strfreev(archs);
    }
  return FALSE;
}
