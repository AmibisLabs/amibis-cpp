#!/bin/sh
#
# This scipt replace path from the OmiscidXXXXX-config
#
#


for file in debian/tmp/usr/bin/*; 
do
	cat ${file} | mawk '{ gsub("debian/tmp", "") ; print }'  >  ${file}
done

