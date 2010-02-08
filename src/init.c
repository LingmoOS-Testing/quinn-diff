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
#include "config.h"
#include "error.h"
#include "getopt.h"
#include "globals.h"
#include "init.h"
#include "parse_packages.h"
#include "parse_sources.h"
#include "utils.h"

void
parse_opt (int argc, char **argv, sources_info *sources, packages_info *packages)
{
  char *debug_argument;
  int c;

  while (1)
    {
      int option_index = 0;
      static struct option long_options[] =
      {
	{"arch-specific-packages", required_argument, NULL, 'a'},
	{"architecture", required_argument, NULL, 'A'},
	{"debug", required_argument, NULL, 'D'},
	{"help", no_argument, NULL, 'h'},
	{"ignore-arch-all", no_argument, NULL, 'i'},
	{"sources", required_argument, NULL, 's'},
	{"packages", required_argument, NULL, 'p'},
	{"version", no_argument, NULL, 'V'},
	{"warn-older", no_argument, NULL, 'w'},
	{0, 0, 0, 0}
      };

      c = getopt_long (argc, argv, "a:A:D:hip:s:w:V",
		       long_options, &option_index);
      if (c == EOF)
	break;

      switch (c)
	{
	case 'a':
	  g_free (packages_arch_specific_filename);
	  packages_arch_specific_filename = xstrdup (optarg);
	  break;

	case 'A':
	  g_free (packages_architecture);
	  packages_architecture = xstrdup (optarg);
	  break;

	case 'D':
	  if (!strcmp (optarg, "help"))
	    debug_help();
	  else
	    {
	      debug_level = strtoul (optarg, &debug_argument, 8);
	      if (*debug_argument)
		{
		  error ("--debug requires an octal argument");
		  usage (1);
		}
	    }
	  break;

	case 'h':
	  usage (0);
	  break;

	case 'i':
	  ignore_arch_all = TRUE;
	  break;

	case 'p':
	  g_free (packages->file_name);
	  packages->file_name = xstrdup (optarg);
	  break;

	case 's':
	  g_free (sources->file_name);
	  sources->file_name = xstrdup (optarg);
	  break;

	case 'w':
	  warn_older = TRUE;
	  break;

	case 'V':
	  printf ("Quinn diff version %s\n", VERSION);
	  exit (0);
	  break;

	case '?':
	  usage(1);
	  break;

	default:
	  break;
	}
    }

   if (optind != argc)
     {
       error ("Quinn diff no longer accepts non-option arguments.");
       usage(1);
     }

}

void
usage (int status)
{
  FILE *f;

  if (status)
    f = stderr;
  else
    f = stdout;

  fprintf (f, "Usage: %s [-apswhDV]\n", program_name);
  fprintf (f, "-a --arch-specific-packages  filename of Packages-arch-specific file.\n");
  fprintf (f, "                              default: /etc/Packages-arch-specific\n");
  fprintf (f, "-A --architecture=<arch>     architecture of Packages file.\n");
  fprintf (f, "                              default is auto-detected\n");
  fprintf (f, "-D --debug=<octal value>     enable debug output; see -Dhelp or --debug=help\n");
  fprintf (f, "-h --help                    give this help\n");
  fprintf (f, "-i --ignore-arch-all         ignore Architecture: all source packages\n");
  fprintf (f, "-p --packages                filename of Packages file.\n");
  fprintf (f, "                              default: Packages in cwd\n");
  fprintf (f, "-s --source-packages         filename of Sources file.\n");
  fprintf (f, "                              default: Sources in cwd\n");
  fprintf (f, "-w --warn-older              warn about packages older in Sources file\n");
  fprintf (f, "-V --version                 display version number\n");
  exit (status);

}
