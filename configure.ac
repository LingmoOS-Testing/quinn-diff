dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([quinn-diff], [0.70], [pkern@debian.org])
AC_CONFIG_AUX_DIR(scripts)
AM_INIT_AUTOMAKE([-Wall std-options check-news -Wno-portability])
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL

dnl Electric Fence; see ftp://ftp.debian.org/pub/debian/dists/unstable/main/source/devel/
opt_efence=no
AC_ARG_WITH(efence, [  --with-efence          compile with electric fence], opt_efence=yes)
if test "$opt_efence" = "yes"; then
  case "x$withval" in
  x/*|x.*)
    EFENCE="$withval"
    ;;
  *)
    EFENCE="/usr/lib/libefence.a"
    ;;
  esac
  if test ! -s "$EFENCE"; then
     AC_MSG_RESULT([Warning: $EFENCE does not exist])
  else
     AC_MSG_RESULT([Compiling with electric fence ($EFENCE)])
  fi
else
  EFENCE=
fi
AC_SUBST(EFENCE)
withval=""

dnl ccmalloc; see http://iseran.ira.uka.de/~armin/ccmalloc/
opt_ccmalloc=no
AC_ARG_WITH(ccmalloc, [  --with-ccmalloc          compile with ccmalloc], opt_ccmalloc=yes)
if test "$opt_ccmalloc" = "yes"; then
  case "x$withval" in
  x/*|x.*)
    CCMALLOC="$withval"
    ;;
  *)
    CCMALLOC="/usr/lib/libccmalloc.a"
    ;;
  esac
  if test ! -s "$CCMALLOC"; then
     AC_MSG_RESULT([Warning: $CCMALLOC does not exist; not using it])
     CCMALLOC=
  else
     AC_MSG_RESULT([Compiling with ccmalloc ($CCMALLOC)])
     CCMALLOC="$CCMALLOC -ldl"
  fi
else
  CCMALLOC=
fi
AC_SUBST(CCMALLOC)
withval=""

dnl Checks for libraries.
AM_PATH_GLIB_2_0()

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h limits.h unistd.h getopt.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
CC_FUNC_EXPECT

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strerror strstr strtol strtoul getopt_long)

AC_OUTPUT([Makefile doc/Makefile src/Makefile example_scripts/Makefile])

