SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt cmake_uninstall.cmake.in src/CMakeLists.txt src/*/CMakeLists.txt src/gui/icons/* extra/wright.ttf extra/uChroma.dt extra/uchroma.png

uchromadir = $(datadir)/uchroma
uchroma_DATA = extra/uchroma.png extra/wright.ttf

desktopdir = @BUILDDIR@/usr/share/applications
desktop_DATA = extra/uChroma.desktop

