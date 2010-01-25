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

#ifndef QUINN_LIST_H
#define QUINN_LIST_H

#include "common.h"

typedef struct _quinn_list Qlist;

struct _quinn_list
{
  void *data;
  Qlist *next;
};

typedef struct _want_node want_node;

struct _want_node 
{
  char search_string[20];
  char **destination_string;
  boolean found;
};

/****** Non-Generic *********/
Qlist *want_append(Qlist *list, want_node *data);
Qlist *want_find(Qlist *list, const char *search_string);
/****** Generic *********/
Qlist *qlist_append(Qlist *list, void *data);
void qlist_destroy(Qlist *list_to_destroy);
/******************/
Qlist *qlist_last (Qlist *list);

#endif /* QUINN_LIST_H */
