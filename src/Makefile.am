bin_PROGRAMS = uchroma

SUBDIRS = gui session templates base math expression kernels render

noinst_HEADERS = version.h

# Target 'uchroma'

uchroma_SOURCES = main.cpp

INCLUDES = -I../ @GUI_CFLAGS@

uchroma_LDADD = gui/libgui.a session/libsession.a base/libbase.a render/librender.a expression/libexpression.a kernels/libkernels.a math/libmath.a @MAC_LIBS@ @GUI_LDLIBS@
