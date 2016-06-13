project(uChroma)

set(DESCRIPTION "uChroma")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "2")
set(VERSION_PATCH "2")

set(CMAKE_BUILD_TYPE "Release")


cmake_minimum_required(VERSION 2.8.12)

if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)

# Add definitions
LIST(APPEND CMAKE_MODULE_PATH "${uChroma_SOURCE_DIR}/cmake/Modules") 
include(DefineOptions)
include(DefineFunctions)
include(GNUInstallDirs)
include(FindBISON)
include(FindFTGL)
include(FindGLEXT)

# Find required packages for this project
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(BISON REQUIRED)
find_package(OpenGL REQUIRED)
find_package(FTGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(GLEXT REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ZLIB REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make sure CMake won't try to generate rules for moc (we will do it ourselves)
set(CMAKE_AUTOMOC OFF)

# Set policy for automatic linkage of Qt libs to project
CMAKE_POLICY(SET CMP0020 NEW)

# Perform system-specific setup
# -- Windows
if(WIN32)
  set (target_name uChroma)

  # Adjust external include directories
  include_directories(
    ${FTGL_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLEXT_INCLUDE_DIRS}
  )

   # Setup HDF5
  SET(HDF5_LIBS "-LC:/src/hdf5-1.8.10-patch1/bin -lhdf5 -L${SZIP_DIR}/bin -lszip" CACHE STRING "Link-time commands to use HDF5 (Windows only)")

  # Add global NOMINMAX define for Windows systems, and inhibit console creation
  ADD_DEFINITIONS("-DNOMINMAX")
  SET(GUI_TYPE WIN32)
  if(MINGW)
    SET(CMAKE_EXE_LINKER_FLAGS "-mwindows")
  endif(MINGW)
endif(WIN32)

# -- Unix
if(UNIX)
  set (target_name uchroma)
endif(UNIX)

# -- OSX
if(APPLE)
  set (target_name uchroma)
  set(CMAKE_OSX_ARCHITECTURES "x86_64")
  ADD_DEFINITIONS(-D_MAC)
  # Find OpenGL and GLUT
  find_package(GLUT REQUIRED)
endif(APPLE)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(SRCS src/)

# Set bundle info
If(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE  "uChroma.icns")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER  "uChroma")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_BUNDLE_NAME  "uChroma" )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION  "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

# Process CMakeLists in subdirs
add_subdirectory(${SRCS})

add_executable(${target_name}
  MACOSX_BUNDLE
  ${SRCS}/main.cpp 
)

# Set project-local include directories for target
target_include_directories(${target_name} PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/gui
  ${FREETYPE_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
)

# Perform final link
set(UCHROMA_LINK_LIBS Qt5::Widgets Qt5::Core ${FTGL_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES} ${READLINE_LIBRARIES} ${HDF5_LIBRARIES} ${FTGL_LIBRARIES})
target_link_libraries( ${target_name}
  gui base render math expression kernels session
  ${UCHROMA_LINK_LIBS}
)

# Install target for unix
if(UNIX AND NOT APPLE)
  INSTALL_TARGET(${target_name} "")
  configure_file(${PROJECT_SOURCE_DIR}/desktop.cmake uChroma.desktop)
  install(FILES ${PROJECT_SOURCE_DIR}/src/gui/icons/uchroma.png
        DESTINATION share/uchroma)
  install(FILES ${PROJECT_BINARY_DIR}/uChroma.desktop
        DESTINATION share/applications)
  foreach(i ff filters fftesting fragments partitions scripts test includes)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/${i}
          DESTINATION share/uchroma)
  endforeach(i)
endif(UNIX AND NOT APPLE)
