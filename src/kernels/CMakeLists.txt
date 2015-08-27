add_library(kernels
  fit.cpp
  fit_sd.cpp
  fit_sdmod.cpp
  fit_simplex.cpp
  fit.h
)

target_include_directories(kernels PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)
