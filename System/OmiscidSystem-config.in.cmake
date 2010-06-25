#! /bin/sh
# @configure_input@
# @EditPossible@

usage()
{
    cat <<EOF
Usage OmiscidSystem-config [OPTION] mode ...

mode:
    --help                print this message
    --cflags              print cflags necessary to compile with primasystem
    --cflags-only-I       print only the -I flags
    --libs                print flags to link with primavision
    --libs-only-L         print only the -L flags (ie the libdir)
    --libs-only-l         print only the -l flags
    --libdir              print name of the directory containing libprimasystem
    --includedir          print name of the directory containing the headers
    --prefix              print the prefix where lib script and headers are
    --doc                 print the path to the html documentation
    --doc-tag             print theline for doxyfile to link with the documentation
    --all                 equivalent to `--cflags --libs`

EOF
    exit $1
}

if test $# -eq 0; then
    usage 1
fi

prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@

while test $# -gt 0; do
case "$1" in
    --help)
	usage 0
	;;
    --cflags)
	echo -n " -I@CMAKE_INSTALL_PREFIX@/include/Omiscid @OMISCID_CXX_FLAGS@"
	;;
    --cflags-only-I)
	echo -n " -I@CMAKE_INSTALL_PREFIX@/include/Omiscid "
	;;
    --libs)
	echo -n " -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ -lOmiscidSystem "
	;;
    --libs-only-L)
	echo -n " -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ "
	;;
    --libs-only-l)
	echo -n " -lOmiscidSystem "
	;;
    --libdir)
	echo -n " @CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ "
	;;
    --includedir)
	echo -n " @CMAKE_INSTALL_PREFIX@/include/Omiscid "
	;;
    --prefix)
	echo -n " $prefix "
	;;
    --doc)
	echo -n " @DOCDIR_SYSTEM@ "
	;;
    --doc-tag)
	echo -n " @DOCDIR_SYSTEM@/doc_system.tag=@DOCDIR_SYSTEM@ "
	;;
    --all)
	echo -n " -I@CMAKE_INSTALL_PREFIX@/include/Omiscid -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ -lOmiscidSystem @OMISCID_CXX_FLAGS@ "
	;;
    *)
	echo "Unknown mode \"$1\""
	echo
	usage 1
	;;
    esac
shift
done
