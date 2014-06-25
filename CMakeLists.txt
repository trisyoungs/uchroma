project(uChroma)

set(DESCRIPTION "uChroma")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "80")

set(CMAKE_BUILD_TYPE "Release")

if(APPLE)
  cmake_minimum_required(VERSION 2.6.0)
  set(CMAKE_OSX_ARCHITECTURES "i386")
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

# Setup SZIP and FTGL (Windows Only)
if(WIN32)
  set(SZIP_DIR "C:/src/szip-2.1")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "C:/src/szip-2.1/")
  set(FTGL_INCLUDES "C:/src/ftgl-2.1.3~rc5/src")
  set(FT2_INCLUDES "C:/Program Files (x86)/GnuWin32/include" "C:/Program Files (x86)/GnuWin32/include/freetype2")
  set(FTGL_LIBRARIES "C:/src/ftgl-2.1.3~rc5/src/libftgl.a")
  set(FT2_LIBRARIES "C:/Program Files (x86)/GnuWin32/lib/libfreetype.dll.a")
  set(GLEXT_INCLUDES "C:/src/include")
endif(WIN32)

# Find required packages for this project
find_package(Qt4 REQUIRED)
IF(WIN32 OR APPLE)
 find_package(OPENGL REQUIRED)
endif(WIN32 OR APPLE)

# Setup HDF5-related stuff
if(MINGW)
	  SET(HDF5_LIBS "-LC:/src/hdf5-1.8.10-patch1/bin -lhdf5 -L${SZIP_DIR}/bin -lszip" CACHE STRING "Link-time commands to use HDF5 (Windows only)")
else(MINGW)
	  SET(HDF5_LIBS "C:/Program Files (x86)/HDF_Group/HDF5/1.8.10/lib/hdf5dll.lib" CACHE STRING "Link-time commands to use HDF5 (Windows only)")
endif(MINGW)

find_package(HDF5 REQUIRED)
find_package(ZLIB REQUIRED)
if(NOT UNIX)
	find_package(SZIP REQUIRED)
endif(NOT UNIX)

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
	set(MACOSX_BUNDLE_ICON_FILE  "uchroma.icns")
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
	target_link_libraries(${target_name} gui base math parser command ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${FTGL_LIBRARIES} ${OPENGL_LIBRARIES} ${FT2_LIBRARIES} ${HDF5_LIBS})
ELSE(WIN32 OR APPLE)
	target_link_libraries(${target_name} gui base math parser command ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${HDF5_LIBRARIES})
ENDIF(WIN32 OR APPLE)

# -----------------------------------
# Installation with CPack starts here
# -----------------------------------

# Add local dir to module path (so we find out local NSIS.template.in)
set(CPACK_MODULE_PATH ${CPACK_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extra")

####################
####install target
####################
if(NOT ${PREFIX})
  MESSAGE("Install folder ${PREFIX}")
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif(NOT ${PREFIX})

if(UNIX AND NOT APPLE)
  configure_file(${CMAKE_SOURCE_DIR}/desktop.cmake ${CMAKE_BINARY_DIR}/uChroma.desktop)
  install(FILES ${CMAKE_BINARY_DIR}/uChroma.desktop 
       DESTINATION "/usr/share/applications/")
  install(FILES ${CMAKE_SOURCE_DIR}/extra/uChroma.png
        DESTINATION share/uchroma)
endif(UNIX AND NOT APPLE)

if (APPLE)
  install(TARGETS ${target_name}
  BUNDLE DESTINATION .
  )
else(APPLE)
  install(TARGETS ${target_name}
      RUNTIME DESTINATION bin
   )
endif(APPLE)

# Install extra data
if (WIN32)
   # Install test data
   install(DIRECTORY  "${CMAKE_HOME_DIRECTORY}/test/" 
     DESTINATION test
     REGEX ".svn" EXCLUDE
   )
   # Install default font
   install(FILES  ${CMAKE_SOURCE_DIR}/extra/wright.ttf 
     DESTINATION bin
   )
endif(WIN32)

if(WIN32)
  Find_File(qtgui QtGui4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtcore QtCore4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtgl QtOpenGL4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtsvg QtSvg4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtxml QtXml4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qsvg qsvg4.dll PATHS ${QT_PLUGINS_DIR}/iconengines NO_DEFAULT_PATH)
  Find_File(mingwgcc libgcc_s_dw2-1.dll PATHS "C:/MinGW/bin" NO_DEFAULT_PATH)
  Find_File(mingwg++ libstdc++-6.dll PATHS "C:/MinGW/bin" NO_DEFAULT_PATH)
  Find_File(ft2 freetype6.dll PATHS "C:/Program Files (x86)/GnuWin32/bin" NO_DEFAULT_PATH)
  Find_File(zlib zlib1.dll PATHS "C:/Program Files (x86)/GnuWin32/bin" NO_DEFAULT_PATH)
  if(NOT qsvg) 
    Find_File(qsvg qsvgicon4.dll PATHS ${QT_PLUGINS_DIR}/iconengines NO_DEFAULT_PATH)
  endif(NOT qsvg)
  Find_File(qsvgie qsvg4.dll PATHS ${QT_PLUGINS_DIR}/imageformats NO_DEFAULT_PATH)
  install(FILES ${qtgui} ${qtcore} ${qtnetwork} ${qtsvg} ${qtxml} ${qtgl} ${mingwgcc} ${mingwg++} ${ft2} ${zlib}  ${CMAKE_CURRENT_SOURCE_DIR}/extra/uChroma.ico
   DESTINATION bin
  )
  install(FILES ${qsvg} 
    DESTINATION bin/iconengines
  )
  install(FILES ${qsvgie} 
    DESTINATION bin/imageformats
  )
ENDIF(WIN32)


if(APPLE)
## install the icon for the bundle
  install(FILES "${CMAKE_SOURCE_DIR}/extra/uchroma.icns" 
        DESTINATION "${target_name}.app/Contents/Resources/")
#  install(DIRECTORY  "${CMAKE_HOME_DIRECTORY}/data/"
#     DESTINATION ${target_name}.app/Contents/SharedSupport/
#     REGEX ".svn" EXCLUDE)
  install(FILES "${CMAKE_SOURCE_DIR}/extra/uchroma.icns"
        DESTINATION . RENAME ".VolumeIcon.icns")
  install(FILES "${CMAKE_SOURCE_DIR}/extra/background.png"
        DESTINATION .background/) 
  install(FILES "${CMAKE_SOURCE_DIR}/COPYING" 
         DESTINATION . RENAME .COPYING)
  install(FILES "${CMAKE_SOURCE_DIR}/extra/uChroma.DS_Store"
         DESTINATION . RENAME .DS_Store)
#  INSTALL(CODE "set(input_file
#        \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${target_name}.app/Contents/MacOS/${target_name}\")")
#  INSTALL(SCRIPT "${CMAKE_SOURCE_DIR}/CMakeIngestOSXBundleLibraries.cmake")
endif(APPLE)

#######################
## uninstall target
#######################

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
###########################

INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
SET(CPACK_PACKAGE_VENDOR ${AUTHOR})
SET(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})

IF (APPLE)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README.txt COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING.txt COPYONLY) 
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_BINARY_DIR}/README.txt")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/COPYING.txt")
ELSE(APPLE)
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING") 
ENDIF(APPLE)

SET(CPACK_PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "uChroma")
IF(WIN32)
  SET(CPACK_GENERATOR "NSIS;ZIP")
  set(CMAKE_INSTALL_PREFIX "")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/uChroma.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/uChroma.ico")
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/extra\\\\uChroma.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\uchroma.ico")  
  SET(CPACK_NSIS_DISPLAY_NAME "uChroma")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.projectaten.net\\\\uchroma")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.projectaten.net\\\\uchroma")
  SET(CPACK_NSIS_CONTACT "tris@projectaten.net")
  SET(CPACK_NSIS_MODIFY_PATH ON) 
  SET(CPACK_PACKAGE_EXECUTABLES "uchroma" "uchroma")
  set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\uchroma.lnk\\\" \\\"$INSTDIR\\\\bin\\\\uchroma.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\uchroma.ico\\\"
      StrCmp $INSTALL_DESKTOP \\\"1\\\" 0 +2
      CreateShortCut \\\"$DESKTOP\\\\uchroma.lnk\\\" \\\"$INSTDIR\\\\bin\\\\uchroma.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\uchroma.ico\\\"
"
  )
Endif(WIN32)

IF(UNIX)
  SET(CPACK_GENERATOR "TGZ;TBZ2;RPM;DEB")
Endif(UNIX)

IF(APPLE)
  SET(CPACK_GENERATOR "TGZ")
Endif(APPLE)

MESSAGE("${CPACK_TEMPORARY_DIRECTORY}")
SET(CPACK_CMAKE_GENERATOR ${CMAKE_GENERATOR})

INCLUDE(CPack)


