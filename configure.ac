dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([MPEG-CDVA-XM], [1.0], [massimo.balestri@telecomitalia.it])
AC_CANONICAL_SYSTEM
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX
AC_PROG_CXX_C_O
AC_PROG_INSTALL

# GNU LibTool provides a portable way to build libraries.  AutoMake
# knows how to use it; you just need to activate it.
LT_INIT
# Turn off shared libraries during development, since they
# make the build process take too long.
# LT_INIT([disable-shared])      
    
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

