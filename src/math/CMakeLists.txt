add_library(math
doubleexp.cpp
mathfunc.cpp
matrix.cpp
constants.h
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
