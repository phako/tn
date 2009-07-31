AUTOMAKE_OPTIONS = dist-bzip2 foreign subdir-objects
NULL =
bin_PROGRAMS = src/tn

src_tn_SOURCES = \
				 src/histogram.c \
				 src/histogram.h \
				 src/image.c \
				 src/image.h \
				 src/tn.c \
				 src/util.c \
				 src/util.h \
				 src/video.c \
				 src/video.h \
				 $(NULL)

AM_CFLAGS = \
			-include config.h \
			$(FFMPEG_CFLAGS) \
			$(LIBGJPEG_CFLAGS) \
			$(CAIRO_CFLAGS) \
			$(NULL)

src_tn_LDFLAGS = \
				 $(FFMPEG_LIBS) \
				 -ljpeg \
				 $(CAIRO_LIBS) \
				 $(NULL)
