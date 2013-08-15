SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt cmake_uninstall.cmake.in src/CMakeLists.txt src/*/CMakeLists.txt src/gui/icons/* extra/wright.ttf

fqplotdir = $(datadir)/fqplot
fqplot_DATA = src/gui/icons/FQPlot.png extra/wright.ttf

desktopdir = @BUILDDIR@/usr/share/applications
desktop_DATA = FQPlot.desktop

