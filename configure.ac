AC_INIT([sko], [1.0], [emcconville@emcconville.com])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CXX([clang++ g++ c++])
AC_PROG_CC([clang gcc cc])
AX_CXX_COMPILE_STDCXX(11, [ext], [mandatory])
AC_CHECK_LIB([ncurses], [initscr], [], [AC_MSG_ERROR([unable to locate libncurses])])
AC_CHECK_HEADER_STDBOOL
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
