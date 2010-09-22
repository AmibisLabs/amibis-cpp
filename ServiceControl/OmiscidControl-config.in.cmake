#! /bin/sh
# @configure_input@
# @EditPossible@

usage()
{
    cat <<EOF
Usage OmiscidControl-config [OPTION] mode ...

mode:
    --help                print this message
    --cflags              print cflags necessary to compile with the control part
    --cflags-only-I       print only the -I flags
    --libs                print flags to link with the control part
    --libs-only-L         print only the -L flags (ie the libdir)
    --libs-only-l         print only the -l flags
    --libdir              print name of the directory containing libcontrol
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

prefix=@prefix@
exec_prefix=@exec_prefix@

while test $# -gt 0; do
case "$1" in
    --help)
	usage 0
	;;
    --cflags)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidCom-config --cflags` `xml2-config --cflags` @zeroconfflag@ "
	;;
    --cflags-only-I)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidCom-config --cflags-only-I` "
	;;
    --libs)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidCom-config --libs` -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ -lOmiscidControl `xml2-config --libs` @zeroconflib@ "
	;;
    --libs-only-L)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidCom-config --libs-only-L` -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ "
	;;
    --libs-only-l)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidCom-config --libs-only-l` -lOmiscidControl "
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
	echo -n " @DOCDIR_CONTROL@ "
	;;
    --doc-tag)
	echo -n " @DOCDIR_CONTROL@/doc_control.tag=@DOCDIR_CONTROL@ "
	;;
    --all)
	echo -n " `@CMAKE_INSTALL_PREFIX@/bin/OmiscidCom-config --libs --cflags-only-I` `xml2-config --cflags` @zeroconfflag@ @zeroconflib@ -L@CMAKE_INSTALL_PREFIX@/lib@LIB_SUFFIX@ -lOmiscidControl `xml2-config --libs` @OMISCID_CXX_FLAGS@"
	;;	    
    *)
	echo "Unknown mode \"$1\""
	echo
	usage 1
	;;
    esac
shift
done