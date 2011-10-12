## Process this file with automake to generate a Makefile.in
## To build all programs with GTK+ uncomment these lines.

AM_CPPFLAGS = -I$(top_srcdir) -I$(includedir) $(GNOME_INCLUDEDIR) \
         -DG_LOG_DOMAIN=\"baskstat\"

bin_PROGRAMS = baskstat

SOURCES = main.c \
		  baskstat-team.h \
		  baskstat-team.c \
		  baskstat-player.h \
		  baskstat-player.c \
		  baskstat-court.h \
		  baskstat-window.h \
		  baskstat-window.c \
		  baskstat-timer.h \
		  baskstat-timer.c \
		  baskstat-court.c

AM_CFLAGS = @GTK_CFLAGS@ \
			 $(PACKAGE_CFLAGS) \
			-DGNOMELOCALEDIR=\"$(datadir)/locale\" \
			-DDATA_DIR=\""$(pkgdatadir)"\"

baskstat_LDADD = @GTK_LIBS@ \
			$(PACKAGE_LIBS)

baskstat_SOURCES = $(SOURCES)

CLEANFILES = *~
