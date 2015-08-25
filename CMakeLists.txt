project(uChroma)

set(DESCRIPTION "uChroma")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "2")

set(CMAKE_BUILD_TYPE "Release")

if(APPLE)
  cmake_minimum_required(VERSION 2.6.0)
  set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
  ADD_DEFINITIONS(-D_MAC)
else(APPLE)
  cmake_minimum_required(VERSION 2.4.8)
endif(APPLE)

if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)

# Set target name
if(WIN32)
	set (target_name uChroma)
else(WIN32)
	set (target_name uchroma)
endif(WIN32)

# Find required packages for this project
find_package(Qt4 REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ZLIB REQUIRED)
include(FindBISON)

# Perform system-specific setup
# -- Windows
if(WIN32)
  # Setup SZIP and FTGL, and GLExt bits and pieces (for custom build machine)
  set(SZIP_DIR "C:/src/szip-2.1")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "C:/src/szip-2.1/")
  set(FTGL_INCLUDES "C:/src/ftgl-2.1.3~rc5/src")
  set(FT2_INCLUDES "C:/GnuWin32/include" "C:/GnuWin32/include/freetype2")
  set(FTGL_LIBRARIES "C:/src/ftgl-2.1.3~rc5/src/libftgl.a")
  set(FT2_LIBRARIES "C:/GnuWin32/lib/libfreetype.dll.a")
  set(GLEXT_INCLUDES "C:/src/include")
  # Find OpenGL
  find_package(OPENGL REQUIRED)
  # Setup HDF5
  SET(HDF5_LIBS "-LC:/src/hdf5-1.8.10-patch1/bin -lhdf5 -L${SZIP_DIR}/bin -lszip" CACHE STRING "Link-time commands to use HDF5 (Windows only)")
endif(WIN32)

# -- Unix
if(UNIX)
  # Set include location for FreeType2
  set(FT2_INCLUDES "/usr/include/freetype2")
endif(UNIX)

# -- OSX
if(APPLE)
  # Set include location for FreeType2
  set(FT2_INCLUDES "/usr/local/include/freetype2")
  # Find OpenGL and GLUT
  find_package(OPENGL REQUIRED)
  find_package(GLUT REQUIRED)
  # Set libraries for link time
  set(FTGL_LIBRARIES "-L/usr/local/lib -lftgl")
  # Add define to suppress gl.h/gl3.h include warnings
  ADD_DEFINITIONS("-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED")
endif(APPLE)

# Set output paths for libraries and binaries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(SRCS src/)

# Add global NOMINMAX define for Windows systems, and inhibit console creation
if(WIN32)
	ADD_DEFINITIONS("-DNOMINMAX")
	SET(GUI_TYPE WIN32)
	if(MINGW)
		SET(CMAKE_EXE_LINKER_FLAGS "-mwindows")
	endif(MINGW)
endif(WIN32)

# Set search locations for header files (for both project and external dependencies)
include_directories(
	${CMAKE_BINARY_DIR}/src
	${CMAKE_SOURCE_DIR}/src
	${QT_INCLUDE_DIR}
	${FTGL_INCLUDES}
	${FT2_INCLUDES}
	${GLEXT_INCLUDES}
	${HDF5_INCLUDE_DIRS}
)

# Process CMakeLists.txt in src subdirectory
add_subdirectory(${SRCS})

# Set bundle information for Mac
If(APPLE)
	set(MACOSX_BUNDLE_ICON_FILE  "uChroma.icns")
	set(MACOSX_BUNDLE_GUI_IDENTIFIER  "uChroma")
	set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
	set(MACOSX_BUNDLE_BUNDLE_NAME  "uChroma" )
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
	set(MACOSX_BUNDLE_BUNDLE_VERSION  "${VERSION_MAJOR}.${VERSION_MINOR}")
	set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

# Tag executable for generation
add_executable(${target_name}  MACOSX_BUNDLE
	${SRCS}/main.cpp 
)

# Link line
link_directories (${LIBRARY_OUTPUT_PATH})
IF(WIN32 OR APPLE)
	target_link_libraries(${target_name} gui base render math expression kernels session ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${FTGL_LIBRARIES} ${OPENGL_LIBRARIES} ${FT2_LIBRARIES} ${HDF5_LIBS})
ELSE(WIN32 OR APPLE)
	target_link_libraries(${target_name} gui base render math expression kernels session ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${HDF5_LIBRARIES} -lGL -lftgl)
ENDIF(WIN32 OR APPLE)

