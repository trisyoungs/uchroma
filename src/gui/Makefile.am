noinst_LIBRARIES = libgui.a

SUFFIXES: .ui .uih .qrc

.ui.o:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/$*.h
	$(CXX) -I$(top_srcdir)/src -I$(top_srcdir)/src/gui/ $(CXXFLAGS) @GUI_CFLAGS@ -c -o $@ moc_$*.cpp
	rm moc_$*.cpp

.uih.o:
	$(QTMOC) -o $*.cpp -f$< $<
	$(CXX) -I$(top_srcdir)/src -I../ -I./ $(CXXFLAGS) @GUI_CFLAGS@ -c -o $@ $*.cpp
	rm $*.cpp

.qrc.o:
	$(QTRCC) -o $*.cpp -name $* $<
	$(CXX) -I$(top_srcdir)/src -I../ -I./ $(CXXFLAGS) $(QTGUI_CFLAGS) -c -o $@ $*.cpp
	rm $*.cpp

clean:
	rm -f *.o ui_* viewer.cpp

libgui_a_SOURCES = icons.qrc fqplot.ui fqplot_funcs.cpp fqplot_data.cpp fqplot_analyse.cpp fqplot_colour.cpp fqplot_sourcedata.cpp fqplot_transform.cpp fqplot_view.cpp

libgui_a_SOURCES += colourbutton.uih colourbutton_funcs.cpp
libgui_a_SOURCES += gradientbar.uih gradientbar_funcs.cpp
libgui_a_SOURCES += plotwidget.uih plotwidget_data.cpp plotwidget_funcs.cpp plotwidget_paintevent.cpp 
libgui_a_SOURCES += viewer.uih viewer_funcs.cpp viewer_input.cpp viewer_prefs.cpp viewer_primitive.cpp viewer_primitiveinfo.cpp viewer_primitivevertex.cpp viewer_textprimitive.cpp viewer_trianglechopper.cpp viewer_user.cpp viewer_vertexchunk.cpp

noinst_HEADERS = fqplot.h viewer_primitive.h viewer_primitiveinfo.h viewer_textprimitive.h viewer_trianglechopper.h viewer_vertexchunk.h

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @GUI_CFLAGS@



