add_library(kernels
  fit.cpp
  fit_sd.cpp
  fit_simplex.cpp
  fit_target.cpp
  fit.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
