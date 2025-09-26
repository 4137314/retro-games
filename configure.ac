AC_INIT([retro-games], [1.0], [your@email.com])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_INSTALL
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([NCURSES], [ncurses], [], [AC_MSG_ERROR([ncurses library not found])])
AC_SUBST([NCURSES_CFLAGS])
AC_SUBST([NCURSES_LIBS])
AC_OUTPUT
