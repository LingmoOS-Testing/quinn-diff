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

#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "list.h"
#include "xmalloc.h"

/****** Non-Generic *********/

Qlist *
want_append(Qlist *list, want_node *data)
{

  want_node *new_data;
  
  new_data = xmalloc (sizeof(want_node));
  *new_data = *data;
  return (qlist_append (list, new_data));

}

Qlist *
want_find(Qlist *list, const char *search_string)
{

  Qlist *pos_in_list;

  pos_in_list = list;
  
  while (pos_in_list)
    {
      if (!strncmp (search_string, 
		    ((want_node *)(pos_in_list->data))->search_string,
		    strlen (search_string)))
	return pos_in_list;
      
      if (pos_in_list->next)
	pos_in_list = pos_in_list->next;
      else
	{
	  fubar (NONSYS, "want_find: found end of list while searching want_list.  Tell James.");
	}
    }

  fubar (NONSYS, "want_find: list was null.");

}

/****** Generic *********/

/* Appends to the list, but only passes a different list back if it
   was passed NULL as a list. */

Qlist * 
qlist_append(Qlist *list, void *data)
{

  Qlist *new_list;
  Qlist *end_of_list;
  
  new_list = xmalloc (sizeof(Qlist));
  new_list->data = data;
  new_list->next = NULL;

  if (list)
    {
      end_of_list = qlist_last (list);
      end_of_list->next = new_list;
      return list;
    }
  else
    return new_list;
}

void 
qlist_destroy(Qlist *list)
{
  
  Qlist *temp = list;

  if (temp->next)
    qlist_destroy (temp->next);
      
  xfree (temp);

}

Qlist *
qlist_last (Qlist *list)
{

  if (list)
    {
      while (list->next)
	list = list->next;
    }
  
  return list;
}
