AC_INIT([sko], [1.0], [emcconville@emcconville.com])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CXX([clang++ g++ c++])
AC_PROG_CC([clang gcc cc])
AX_CXX_COMPILE_STDCXX(11, [ext], [mandatory])
AC_CHECK_HEADERS([ncurses.h], [], [AC_MSG_ERROR([unable to find ncurses.h])])
AC_SEARCH_LIBS([initscr],[ncurses], [], [AC_MSG_ERROR([unable to find ncurses library])])
AC_CHECK_HEADER_STDBOOL
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
