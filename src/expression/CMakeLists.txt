bison_target(ExpressionParser expression_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/expression_grammar.cc COMPILE_FLAGS -y )

add_library(expression
  ${BISON_ExpressionParser_OUTPUTS}
  expression.cpp
  expression_lexer.cpp
  functionnode.cpp
  functions.cpp
  node.cpp
  variable.cpp
  variablenode.cpp
  expression.h
  functionnode.h
  functions.h
  node.h
  variable.h
  variablenode.h
)

include_directories(
../
${QT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
${CMAKE_SOURCE_DIR}/src/gui
)
