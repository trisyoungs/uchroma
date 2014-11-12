noinst_LIBRARIES = libexpression.a

AM_YFLAGS = -d

clean-local:
	-rm -f expression_grammar.cc

libexpression_a_SOURCES = expression_grammar.yy
libexpression_a_SOURCES += expression.cpp expression_lexer.cpp functionnode.cpp functions.cpp node.cpp variable.cpp variablenode.cpp

noinst_HEADERS = expression.h functionnode.h functions.h node.h variable.h variablenode.h

libexpression_a_CPPFLAGS = -I$(top_srcdir)/src -I../  @GUI_CFLAGS@

