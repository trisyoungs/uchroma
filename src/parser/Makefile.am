noinst_LIBRARIES = libparser.a

AM_YFLAGS = -d

clean-local:
	-rm -f grammar.cc

libparser_a_SOURCES = commandnode.cpp double.cpp grammar.yy integer.cpp parser.cpp parser_lexer.cpp parser_tree.cpp returnvalue.cpp scopenode.cpp stepnode.cpp tree.cpp tree_opcheck.cpp treenode.cpp variable.cpp variablelist.cpp variablenode.cpp vtypes.cpp

noinst_HEADERS = accessor.h commandnode.h double.h integer.h parser.h returnvalue.h scopenode.h stepnode.h tree.h treenode.h variable.h variablelist.h variablenode.h vtypes.h 

INCLUDES = -I$(top_srcdir)/src -I../ 
