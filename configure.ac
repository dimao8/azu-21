#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([azu21], [0.1.0.0], [dimao8@yandex.ru])
AC_CONFIG_SRCDIR([src/azu21.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects silent-rules])

# Check for GCC

ac_ldflags_save="${LDFLAGS}"
LDFLAGS="${LDFLAGS} -specs=nosys.specs -nostartfiles"
AC_PROG_CC
LDFLAGS="${ac_ldflags_save}"

# Debug
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug],
                              [Set to build debug version])])

AS_IF([test "x$enable_debug" = "xyes"],
      [CFLAGS="$CFLAGS -O0 -gdwarf-2 -DDEBUG"],
      [CFLAGS="$CFLAGS -O2 -DNDEBUG"])

AC_CONFIG_FILES([Makefile
                 program.sh
                 connect.sh],
                 [chmod +x program.sh
                  chmod +x connect.sh])
AC_OUTPUT
