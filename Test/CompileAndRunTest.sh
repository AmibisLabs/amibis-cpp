#! /bin/sh

WorkingRep=$1
TestFiles=$2
Compiler=$3
CompilerOption=$4
InvoqueDebugger=$5

export LD_LIBRARY_PATH=/tmp/OmiscidInstall/lib/:./:$LD_LIBRARY_PATH:./:/tmp/OmiscidInstall/lib/
export DYLD_LIBRARY_PATH=/tmp/OmiscidInstall/lib/:./:$DYLD_LIBRARY_PATH:./:/tmp/OmiscidInstall/lib/
export PATH=/tmp/OmiscidInstall/bin:./:$PATH:/softs/stow/insure++-7.1.2/bin/:./:/tmp/OmiscidInstall/bin
export OMISCID_WORKING_DOMAIN=_bip_test_valid._tcp

cd /tmp/$WorkingRep/Examples
rm -f ./Test
# echo $PATH
$3 -o Test $TestFiles `/tmp/OmiscidInstall/bin/OmiscidControl-config --all` $4

cd /tmp/OmiscidInstall/lib/
$5 /tmp/$WorkingRep/Examples/Test
