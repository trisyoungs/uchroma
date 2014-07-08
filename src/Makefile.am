bin_PROGRAMS = uchroma

SUBDIRS = templates base math parser command render gui

noinst_HEADERS = version.h

# Target 'uchroma'

uchroma_SOURCES = main.cpp

INCLUDES = -I../ @GUI_CFLAGS@

uchroma_LDADD = gui/libgui.a base/libbase.a render/librender.a parser/libparser.a command/libcommand.a math/libmath.a @MAC_LIBS@ @GUI_LDLIBS@
