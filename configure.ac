AC_PREREQ(2.60)
AC_INIT([baskstat], [1.0], [danigm@wadobo.com])
AC_CONFIG_AUX_DIR([build])
AM_INIT_AUTOMAKE([1.9.6 -Wall -Werror dist-bzip2])

GNOME_COMMON_INIT

AC_PROG_CC
# Compiling sources with per-target flags requires AM_PROG_CC_C_O
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AM_PATH_GTK_3_0([3.0.0],,AC_MSG_ERROR([Gtk+ 3.0.0 or higher required.]))
PKG_CHECK_MODULES(PACKAGE, "gtk+-3.0 librsvg-2.0 json-glib-1.0")

# *******************************
# Internationalization
# *******************************

AC_PROG_INTLTOOL
GETTEXT_PACKAGE=baskstat
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    data/Makefile
    po/Makefile.in
])

AC_OUTPUT
