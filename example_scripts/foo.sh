#! /opt/SBA/jjtroup/bin/bash

primary_architecture=$(grep "^Architecture: " Packages-primary | grep -v "Architecture: all" | sed -e "s/Architecture: //"  | head -n 1)
secondary_architecture=$(grep "^Architecture: " Packages-secondary | grep -v "Architecture: all" | sed -e "s/Architecture: //"  | head -n 1)

# This is slow; there has to be a better way.
egrep -v "^#|^$|^[ \t]" Packages-arch-specific | grep -v $secondary_architecture | grep $primary_architecture | sed -e "s/:.*//" > qwqtmpwer.1
grep "^Package:" Packages-primary | sed -e "s/Package: //" > qwqtmpwer.2
for i in $(cat qwqtmpwer.1); do grep -v $i qwqtmpwer.2 > qwqtmpwer.3; mv qwqtmpwer.3 qwqtmpwer.2; done
number_of_packages=$(cat qwqtmpwer.2 | wc -l )

number_unported=$(./quinn2.sh | grep "has not been compiled for" | wc -l)
number_out_of_date=$(./quinn2.sh | grep -v "has not been compiled for" | wc -l)

# We don't want bc splitting lines, as this is going into a HTML doc.

export BC_LINE_LENGTH=120 

result=$(echo $number_of_packages $number_unported $number_out_of_date | bc foo.bc)
percent_ported=$(echo $result | awk '{print $1}')
number_ported=$(echo $result | awk '{print $2}')
percent_uptodate=$(echo $result | awk '{print $3}')
number_uptodate=$(echo $result | awk '{print $4}')

printf "<P>There are %d Debian packages in main which are compilable on $secondary_architecture.  %0.2f%% (%d) have been ported, and of those, %0.2f%% (%d) are up-to-date.</P>\n" \
	    $number_of_packages $percent_ported $number_ported $percent_uptodate $number_uptodate


