add_library(math
  cuboid.cpp
  doubleexp.cpp
  mathfunc.cpp
  matrix.cpp
  constants.h
  cuboid.h
  doubleexp.h
  mathfunc.h
  matrix.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
