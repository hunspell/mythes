#!/bin/bash
# Run this to generate all the initial makefiles, etc.

usage() {
  echo >&2 "\
Usage: $0 [OPTION]...
Generate makefiles and other infrastructure needed for building


Options:
 --help                   Print this message

Running without arguments will suffice in most cases.
"
}


set -e
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

cd $srcdir

for option
do
  case $option in
  --help)
    usage
    exit;;
  *)
    ;;
  esac
done

#Check for OSX
case `uname -s` in
Darwin) LIBTOOLIZE=glibtoolize;;
*) LIBTOOLIZE=libtoolize;;
esac


DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile mythes."
	echo "Download the appropriate package for your distribution,"
	echo "or see http://www.gnu.org/software/autoconf"
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	DIE=1
	echo "You must have automake installed to compile mythes."
	echo "Download the appropriate package for your distribution,"
	echo "or see http://www.gnu.org/software/automake"
}

if test "$DIE" -eq 1; then
	exit 1
fi

$LIBTOOLIZE --copy --force
aclocal -I m4
autoheader
automake --add-missing --foreign
autoconf
