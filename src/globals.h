#include "common.h"

/* GLOBAL is defined to empty in `main.c' only, and left alone in
   other `*.c' modules.  Here, we merely set it to "extern" if it is
   not already set. */

#ifndef GLOBAL
# define GLOBAL extern
#endif

/* Name of this program.  */
GLOBAL const char *program_name;

/* If true warn about source packages even if they are Architecture: all  */
GLOBAL boolean ignore_arch_all;

/* If true warn about packages which are older on the ftp site than the version installed */
GLOBAL boolean warn_older;

/* Debug level */
GLOBAL unsigned long debug_level;

/* The secondary architecture's name */
GLOBAL char *packages_architecture;

/* The filename for the Packages-arch-specific file */
GLOBAL char *packages_arch_specific_filename;

/* The filename for the Packages-arch-specific file */
GLOBAL char *packages_source_filename;

/* Output format */
GLOBAL int output_format;

