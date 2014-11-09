add_library(session
  keywords.cpp
  load.cpp
  save.cpp
  session.cpp
  session.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
