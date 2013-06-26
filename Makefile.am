SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt cmake_uninstall.cmake.in src/CMakeLists.txt src/*/CMakeLists.txt

#fqplotdir = $(datadir)/fqplot
#fqplot_DATA = extra/fqplot.png

#desktopdir = @BUILDDIR@/usr/share/applications
#desktop_DATA = FQPLOT.desktop

