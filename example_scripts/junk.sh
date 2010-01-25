#! /bin/bash

# Daily cronjob script for auto-generating parts of the Quinn Diff webpages.
# Copyright (C) 1997, 1998 James Troup <james@nocrew.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

set -e

by_priority()
{
  echo "quinn.sh run @ $(date)" > qd-output/$1/by_priority-$2.txt.new
  ./scripts/quinn-diff_sort_by_priority >> qd-output/$1/by_priority-$2.txt.new
  mv qd-output/$1/by_priority-$2.txt.new qd-output/$1/by_priority-$2.txt
  chmod 644 qd-output/$1/by_priority-$2.txt
}

by_section()
{
  echo "quinn.sh run @ $(date)" > qd-output/$1/by_section-$2.txt.new
  ./scripts/quinn-diff_sort_by_section >> qd-output/$1/by_section-$2.txt.new
  mv qd-output/$1/by_section-$2.txt.new qd-output/$1/by_section-$2.txt
  chmod 644 qd-output/$1/by_section-$2.txt
}

by_priority_split()
{
  echo "quinn.sh run @ $(date)" > qd-output/$1/by_priority_split-$2.txt.new
  ./scripts/quinn-diff_split_by_status $1 >> qd-output/$1/by_priority_split-$2.txt.new
  mv qd-output/$1/by_priority_split-$2.txt.new qd-output/$1/by_priority_split-$2.txt
  chmod 644 qd-output/$1/by_priority_split-$2.txt
}

by_section_html()
{
  sections="admin base comm devel doc editors electronics games graphics hamradio interpreters libs mail math misc net news oldlibs otherosfs shells sound tex text utils web x11"

  test -d qd-output/$1/by_section-$2 || mkdir qd-output/$1/by_section-$2

  quinn-diff > quinn_blah

  for k in $sections; do 
    #echo "[Processing "$k"]";
    rm -f qd-output/$1/by_section-$2/$k.html
    cat > qd-output/$1/by_section-$2/$k.html <<EOF
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN"> 
<HTML>
<HEAD>
  <TITLE>Quinn Diff output for Debian GNU/Linux $1 ($k)</TITLE>
  <LINK REV=MADE HREF="mailto:james@nocrew.org">
</HEAD>
<BODY>
<P ALIGN=CENTER><H1>Quinn Diff output for Debian/$1 $k/</H1></P>
EOF
    egrep "^$k\/" quinn_blah |   sed -e "s#^$k/##" -e "s#_# #" |   awk '{ printf "<P>"
				  printf $0" "
				printf "</P>\n"
			    }' > blah
    NUM_MISSING=$(grep ":uncompiled" blah | wc --lines)
    NUM_OLD=$(grep ":out-of-date" blah | wc --lines)
    cat blah >> qd-output/$1/by_section-$2/$k.html
    echo "<BR><P>There are"$NUM_MISSING" missing packages and"$NUM_OLD" out of date packages</P>" >> qd-output/$1/by_section-$2/$k.html
    cat >> qd-output/$1/by_section-$2/$k.html <<EOF
<HR>
<BR>
Last modified: $(date "+%A, %d %B %Y")<BR>
Please send questions and comments to <A HREF="mailto:james@nocrew.org">james@nocrew.org</A><BR>
Copyright (C) 1998 James Troup &lt;james@nocrew.org&gt;.<BR>
Copying, with or without modification, and distribution is permitted in any medium, provided this notice is preserved.<BR>
</BODY>
</HTML>
EOF
    chmod 644 qd-output/$1/by_section-$2/$k.html
  done
  rm -f quinn_blah blah
}

echo "junk.sh run @ $(date)"

test -d scripts || exit 1;

STABLE_ARCHS="m68k"
FROZEN_ARCHS="m68k" # alpha
UNSTABLE_ARCHS="m68k alpha powerpc sparc arm hurd-i386"

for i in frozen unstable; do
  echo "Pocessing $i distribution...."

  ./scripts/snarf_Packages-primary -d $i -a i386
  ./scripts/snarf_Packages-source -d $i

  if [ $i = stable ]; then
    ARCHS="$STABLE_ARCHS"
  elif [ $i = frozen ]; then
    ARCHS="$FROZEN_ARCHS"
  elif [ $i = unstable ]; then
    ARCHS="$UNSTABLE_ARCHS"
  fi

  for j in $ARCHS; do 
    echo " Processing $j..."
    ./scripts/snarf_Packages-secondary -a $j -d $i
    by_priority $i $j
    by_priority_split $i $j
    by_section $i $j
    by_section_html $i $j;
  done

done

touch index.phtml
make

rm -f Packages-primary Packages-secondary Packages-source
