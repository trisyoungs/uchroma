bison_target(TextPrimitiveParser textprimitive_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/textprimitive_grammar.cc COMPILE_FLAGS -y )

add_library(render
  ${BISON_TextPrimitiveParser_OUTPUTS}
  fontinstance.cpp
  linestipple.cpp
  linestyle.cpp
  primitive.cpp
  primitiveinfo.cpp
  primitiveinstance.cpp
  primitivelist.cpp
  surface.cpp
  surface_grid.cpp
  surface_linexy.cpp
  surface_linezy.cpp
  surface_full.cpp
  textformat.cpp
  textfragment.cpp
  textprimitive.cpp
  textprimitivelist.cpp
  vertexchunk.cpp
  fontinstance.h
  linestipple.h
  linestyle.h
  primitive.h
  primitiveinfo.h
  primitiveinstance.h
  primitivelist.h
  surface.h
  textformat.h
  textfragment.h
  textprimitive.h
  textprimitivelist.h
  vertexchunk.h
)

include_directories(
./
../
${CMAKE_CURRENT_BINARY_DIR} 
${CMAKE_CURRENT_BINARY_DIR}/../
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src/
)

