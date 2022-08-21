# Process this file with autoconf to create configure.

AC_PREREQ([2.65])

# ====================
# Version informations
# ====================
m4_define([mythes_version_major],[1])
m4_define([mythes_version_minor],[2])
m4_define([mythes_version_micro],[6])
m4_define([mythes_version],[mythes_version_major.mythes_version_minor.mythes_version_micro])

# =============
# Automake init
# =============
AC_INIT([mythes],[mythes_version])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_LANG([C++])

# ===========================
# Find required base packages
# ===========================
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT([win32-dll disable-static pic-only])
AC_CANONICAL_HOST

# =================================
# Libtool/Version Makefile settings
# =================================
AC_SUBST(MYTHES_MAJOR_VERSION, [mythes_version_major])
AC_SUBST(MYTHES_MINOR_VERSION, [mythes_version_minor])
AC_SUBST(MYTHES_MICRO_VERSION, [mythes_version_micro])
AC_SUBST(MYTHES_VERSION, [mythes_version])

# Libtool information
# Bump current when changing interfaces and null others
# Bump revision if there was any code change
# Bump age if interfaces were only added and no other change was done
AC_SUBST(LT_CURRENT, [0])
AC_SUBST(LT_REVISION, [0])
AC_SUBST(LT_AGE, [0])

# ===============
# Find librevenge
# ===============
PKG_CHECK_MODULES([HUNSPELL],[
        hunspell
])
AC_SUBST([HUNSPELL_CFLAGS])
AC_SUBST([HUNSPELL_LIBS])


# ================
# Check for cflags
# ================
AC_ARG_ENABLE([werror],
        [AS_HELP_STRING([--disable-werror], [Treat all warnings as errors,
usefull for development])],
        [enable_werror="$enableval"],
        [enable_werror=yes]
)
AS_IF([test x"$enable_werror" != "xno"], [
        CFLAGS="$CFLAGS -Werror"
        CXXFLAGS="$CXXFLAGS -Werror"
])

# ============
# Debug switch
# ============
AC_ARG_ENABLE([debug],
        [AS_HELP_STRING([--enable-debug], [Turn on debugging])],
        [enable_debug="$enableval"],
        [enable_debug=no]
)
AS_IF([test "x$enable_debug" = "xyes"], [
        DEBUG_CXXFLAGS="-DDEBUG -g"
], [
        DEBUG_CXXFLAGS="-DNDEBUG"
])
AC_SUBST(DEBUG_CXXFLAGS)

# =====================
# Prepare all .in files
# =====================
AC_CONFIG_FILES([
Makefile
mythes.pc
])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
        debug:           ${enable_debug}
        werror:          ${enable_werror}
==============================================================================
])
