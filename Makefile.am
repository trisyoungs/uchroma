SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt cmake_uninstall.cmake.in src/CMakeLists.txt src/*/CMakeLists.txt src/gui/icons/* extra/uChroma.dt extra/uchroma.png resources/* src/gui/freefont/*

uchromadir = $(datadir)/uchroma
uchroma_DATA = extra/uchroma.png resources/wright.ttf

desktopdir = @BUILDDIR@/usr/share/applications
desktop_DATA = extra/uChroma.desktop

icondir = @BUILDDIR@/usr/share/pixmaps
icon_DATA = extra/uchroma.png

