#!/bin/sh
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

ORIGDIR=`pwd`
cd $srcdir

AUTORECONF=`which autoreconf`
if test -z $AUTORECONF; then
  echo "*** No autoreconf found, please install it ***"
  exit 1
fi

autoreconf --install --verbose || exit $?
cd $ORIGDIR
