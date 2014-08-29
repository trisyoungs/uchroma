noinst_LIBRARIES = libexpression.a

AM_YFLAGS = -d

clean-local:
	-rm -f grammar.cc

libexpression_a_SOURCES = expression.cpp expression_lexer.cpp functionnode.cpp functions.cpp grammar.yy node.cpp variable.cpp variablenode.cpp

noinst_HEADERS = expression.h functionnode.h functions.h node.h variable.h variablenode.h

INCLUDES = -I$(top_srcdir)/src -I../  @GUI_CFLAGS@

