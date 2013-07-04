bin_PROGRAMS = fqplot

SUBDIRS = templates base math gui

noinst_HEADERS = version.h

# Target 'fqplot'

fqplot_SOURCES = main.cpp

INCLUDES = -I../ @GUI_CFLAGS@

fqplot_LDADD = gui/libgui.a math/libmath.a base/libbase.a @MAC_LIBS@ @GUI_LDLIBS@
