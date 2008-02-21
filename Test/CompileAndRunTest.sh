#! /bin/sh

WorkingRep=$1
TestFiles=$2

export LD_LIBRARY_PATH=/tmp/OmiscidInstall/lib/:./:$LD_LIBRARY_PATH:./:/tmp/OmiscidInstall/lib/
export DYLD_LIBRARY_PATH=/tmp/OmiscidInstall/lib/:./:$DYLD_LIBRARY_PATH:./:/tmp/OmiscidInstall/lib/
export PATH=/tmp/OmiscidInstall/bin:./:$PATH:./:/tmp/OmiscidInstall/bin
export OMISCID_WORKING_DOMAIN=_bip_test_valid._tcp

cd /tmp/$WorkingRep/Examples
rm -f ./Test
# echo $PATH
g++ -o Test $TestFiles `/tmp/OmiscidInstall/bin/OmiscidControl-config --all`

cd /tmp/OmiscidInstall/lib/
/tmp/$WorkingRep/Examples/Test
