#! /bin/sh

WorkingRep=$1
Options=$2

# change dir
cd /tmp/$WorkingRep/

#ask compilation
scons $Options

