bin_PROGRAMS = uchroma

SUBDIRS = gui session templates base math expression kernels render

noinst_HEADERS = version.h

# Target 'uchroma'

uchroma_SOURCES = main.cpp

uchroma_CPPFLAGS = -I../ @UCHROMA_CFLAGS@

uchroma_LDADD = gui/libgui.a base/libbase.a render/librender.a expression/libexpression.a kernels/libkernels.a math/libmath.a session/libsession.a @UCHROMA_LDLIBS@
