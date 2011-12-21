#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_pattern_allow

AC_PREREQ(2.59)
AC_INIT([mythes],[1.2.1],[nemeth@openoffice.org])

AC_CANONICAL_SYSTEM
AC_SUBST(XFAILED)

AM_INIT_AUTOMAKE(mythes, 1.2.1)
MYTHES_VERSION_MAJOR=`echo $VERSION | cut -d"." -f1`
MYTHES_VERSION_MINOR=`echo $VERSION | cut -d"." -f2`
AC_SUBST(MYTHES_VERSION_MAJOR)
AC_SUBST(MYTHES_VERSION_MINOR)

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL

PKG_CHECK_MODULES(HUNSPELL, [hunspell])

AC_CONFIG_FILES([Makefile mythes.pc])

AC_OUTPUT
