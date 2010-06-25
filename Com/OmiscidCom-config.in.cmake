#! /bin/sh
# @configure_input@
# @EditPossible@

usage()
{
    cat <<EOF
Usage OmiscidCom-config [OPTION] mode ...

mode:
    --help                print this message
    --cflags              print cflags necessary to compile with primavision
    --cflags-only-I       print only the -I flags
    --libs                print flags to link with primavision
    --libs-only-L         print only the -L flags (ie the libdir)
    --libs-only-l         print only the -l flags
    --libdir              print name of the directory containing libprimavision
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
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidSystem-config --cflags` "
	;;
    --cflags-only-I)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidSystem-config --cflags-only-I` "
	;;
    --libs)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidSystem-config --libs` -L@libdir@ -lOmiscidCom "
	;;
    --libs-only-L)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidSystem-config --libs-only-L` -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ "
	;;
    --libs-only-l)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidSystem-config --libs-only-l` -lOmiscidCom "
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
	echo -n " @DOCDIR_COM@ "
	;;
    --doc-tag)
	echo -n " @DOCDIR_COM@/doc_com.tag=@DOCDIR_COM@ "
	;;
    --all)
	echo " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidSystem-config --libs --cflags-only-I` -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ -lOmiscidCom @OMISCID_CXX_FLAGS@ "
	;;

	   
    *)
	echo "Unknown mode \"$1\""
	echo
	usage 1
	;;
    esac
shift
done
