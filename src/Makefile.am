bin_PROGRAMS = uchroma

SUBDIRS = gui templates base math expression kernels render

noinst_HEADERS = version.h

# Target 'uchroma'

uchroma_SOURCES = main.cpp

INCLUDES = -I../ @GUI_CFLAGS@

uchroma_LDADD = gui/libgui.a base/libbase.a render/librender.a expression/libexpression.a kernels/libkernels.a math/libmath.a @MAC_LIBS@ @GUI_LDLIBS@
