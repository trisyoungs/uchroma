add_library(kernels
  fit.cpp
  fit_sd.cpp
  fit_sdmod.cpp
  fit_simplex.cpp
  fit.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
