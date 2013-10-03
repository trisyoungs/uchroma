SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt cmake_uninstall.cmake.in src/CMakeLists.txt src/*/CMakeLists.txt src/gui/icons/* extra/wright.ttf

uchromadir = $(datadir)/uchroma
uchroma_DATA = src/gui/icons/uChroma.png extra/wright.ttf

desktopdir = @BUILDDIR@/usr/share/applications
desktop_DATA = uChroma.desktop

