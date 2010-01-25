/* libdpkg - Debian packaging suite library routines
 * vercmp.c - comparison of version numbers
 *
 * Cut to shreds and altered for Quinn Diff.
 *
 * Copyright (C) 1995 Ian Jackson <iwj10@cus.cam.ac.uk>
 * Copyright (C) 1997-2001 James Troup <james@nocrew.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "error.h"
#include "xmalloc.h"
#include "nfmalloc.h"
#include "vercmp.h"

struct versionrevision source_version;
struct versionrevision binary_version;

struct versionrevision {
  unsigned long epoch;
  char *version, *revision;
};

int versioncompare(const struct versionrevision *version,
                   const struct versionrevision *refversion);

const char *parseversion(struct versionrevision *rversion, const char *string);

/* assume ascii; warning: evaluates x multiple times! */
#define order(x) ((x) == '~' ? -1 \
		: cisdigit((x)) ? 0 \
		: !(x) ? 0 \
		: cisalpha((x)) ? (x) \
		: (x) + 256)

static int cisdigit(int c) {
	return (c>='0') && (c<='9');
}

static int cisalpha(int c) {
	return ((c>='a') && (c<='z')) || ((c>='A') && (c<='Z'));
}

static int verrevcmp(const char *val, const char *ref) {
  if (!val) val= "";
  if (!ref) ref= "";

  while (*val || *ref) {
    int first_diff= 0;

    while ( (*val && !cisdigit(*val)) || (*ref && !cisdigit(*ref)) ) {
      int vc= order(*val), rc= order(*ref);
      if (vc != rc) return vc - rc;
      val++; ref++;
    }

    while ( *val == '0' ) val++;
    while ( *ref == '0' ) ref++;
    while (cisdigit(*val) && cisdigit(*ref)) {
      if (!first_diff) first_diff= *val - *ref;
      val++; ref++;
    }
    if (cisdigit(*val)) return 1;
    if (cisdigit(*ref)) return -1;
    if (first_diff) return first_diff;
  }
  return 0;
}

int versioncompare(const struct versionrevision *version,
                   const struct versionrevision *refversion) {
  int r;

  if (version->epoch > refversion->epoch) return 1;
  if (version->epoch < refversion->epoch) return -1;
  r= verrevcmp(version->version,refversion->version);  if (r) return r;
  return verrevcmp(version->revision,refversion->revision);
}

const char *parseversion(struct versionrevision *rversion, const char *string) {
  char *hyphen, *colon, *eepochcolon;
  unsigned long epoch;

  if (!*string) return ("version string is empty");

  colon= strchr(string,':');
  if (colon) {
    epoch= strtoul(string,&eepochcolon,10);
    if (colon != eepochcolon) return ("epoch in version is not number");
    if (!*++colon) return ("nothing after colon in version number");
    string= colon;
    rversion->epoch= epoch;
  } else {
    rversion->epoch= 0;
  }
  rversion->version= nfstrsave(string);
  hyphen= strrchr(rversion->version,'-');
  if (hyphen) *hyphen++= 0;
  rversion->revision= hyphen ? hyphen : nfstrsave("");

  return 0;
}

/* ======= End of iwj's stuff, start of my rubbish ====== */

int
compare_versions (const char *source_version_string,
		  const char *binary_version_string,
		  const char *package)
{
  const char *error_msg;

  error_msg = parseversion (&source_version, source_version_string);
  if (error_msg)
    fubar (NONSYS, "parseversion of %s for package %s (primary): %s",
	   source_version_string, package, error_msg);

  error_msg = parseversion (&binary_version, binary_version_string);
  if (error_msg)
    fubar (NONSYS, "parseversion of %s for package %s (secondary): %s",
	   binary_version_string, package, error_msg);

  debug (debug_versions,
	 "comparing %s: [Source] %ld:%s-%s (was %s) versus [Binary] %ld:%s-%s (was %s), result is %d",
	 package, source_version.epoch, source_version.version, source_version.revision,
	 source_version_string, binary_version.epoch, binary_version.version, binary_version.revision,
	 binary_version_string, versioncompare (&binary_version, &source_version));

  return (versioncompare (&binary_version, &source_version));
}
