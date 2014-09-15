noinst_LIBRARIES = librender.a

AM_YFLAGS = -d

clean-local:
	-rm -f textprimitive_grammar.cc

librender_a_SOURCES = textprimitive_grammar.yy
librender_a_SOURCES += fontinstance.cpp glextensions.cpp linestipple.cpp linestyle.cpp primitive.cpp primitiveinfo.cpp primitiveinstance.cpp primitivelist.cpp surface.cpp surface_grid.cpp surface_linexy.cpp surface_linezy.cpp surface_full.cpp textformat.cpp textfragment.cpp textprimitive.cpp textprimitivelist.cpp vertexchunk.cpp

noinst_HEADERS = fontinstance.h glextensions.h linestipple.h linestyle.h primitive.h primitiveinfo.h primitiveinstance.h primitivelist.h surface.h textformat.h textfragment.h textprimitive.h textprimitivelist.h vertexchunk.h

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @GUI_CFLAGS@

