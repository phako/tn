AC_INIT([tn], [0.2], [mail@jensge.org], [tn])
AC_CONFIG_SRCDIR([src/tn.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign subdir-objects])
AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_C_O

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

PKG_CHECK_MODULES(CAIRO, [cairo],
                  AC_DEFINE(HAVE_CAIRO, 1, [Define if cairo is available]))
PKG_CHECK_MODULES(FFMPEG, [libavformat libavcodec libswscale])
AVFORMAT_DIR=$(pkg-config --variable=includedir libavformat)
AC_CHECK_FILE([${AVFORMAT_DIR}/avformat.h],,
              AC_CHECK_FILE([${AVFORMAT_DIR}/libavformat/avformat.h],
              AC_DEFINE(AVCODEC_NEW_INCLUDE, 1,
                        [Define if ffmpeg uses new include layout]),
              AC_MSG_ERROR([Unable to find ffmpeg include dir])))
AC_CHECK_HEADER([jpeglib.h],
                AC_DEFINE(HAVE_JPEG, 1, [Define if libjpeg is available]))
AC_OUTPUT([Makefile])
