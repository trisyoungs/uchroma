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

clean-local:
	-rm -f ui_* colourbutton.cpp gradientbar.cpp icons.cpp plotwidget.cpp viewer.cpp 

libgui_a_SOURCES = icons.qrc graphwidget.ui texponentialspin.uih
libgui_a_SOURCES += texponentialspin.uih texponentialspin_funcs.cpp
libgui_a_SOURCES += axes.ui create.ui data.ui dataimport.ui equationselect.ui fit.ui saveimage.ui slicemonitor.ui slices.ui style.ui transform.ui view.ui
libgui_a_SOURCES += uchroma.ui uchroma_collection.cpp uchroma_data.cpp uchroma_funcs.cpp uchroma_info.cpp uchroma_interaction.cpp uchroma_slices.cpp uchroma_slots.cpp
libgui_a_SOURCES += io_load.cpp io_save.cpp keywords.cpp
libgui_a_SOURCES += plotwidget.uih plotwidget_data.cpp plotwidget_funcs.cpp plotwidget_paintevent.cpp
libgui_a_SOURCES += axes_funcs.cpp create_funcs.cpp data_funcs.cpp dataimport_funcs.cpp dataimport_seqxy.cpp equationselect_funcs.cpp fit_funcs.cpp fit_main.cpp fit_sd.cpp fit_simplex.cpp fit_target.cpp graphwidget_funcs.cpp saveimage_funcs.cpp slicemonitor_funcs.cpp slices_funcs.cpp style_funcs.cpp transform_funcs.cpp view_funcs.cpp
libgui_a_SOURCES += colourbutton.uih colourbutton_funcs.cpp
libgui_a_SOURCES += gradientbar.uih gradientbar_funcs.cpp
libgui_a_SOURCES += viewer.uih viewer_funcs.cpp viewer_input.cpp viewer_prefs.cpp viewer_primitive.cpp viewer_primitive_vertex.cpp viewer_primitiveinfo.cpp viewer_primitivelist.cpp viewer_textprimitive.cpp viewer_user.cpp viewer_user_axes.cpp viewer_user_grid.cpp viewer_user_linexy.cpp viewer_user_linezy.cpp viewer_user_surface.cpp viewer_vertexchunk.cpp

noinst_HEADERS = uchroma.h axes.h create.h data.h dataimport.h equationselect.h fit.h graphwidget.h keywords.h saveimage.h slicemonitor.h slices.h style.h transform.h view.h viewer_primitive.h viewer_primitiveinfo.h viewer_primitivelist.h viewer_textprimitive.h viewer_vertexchunk.h

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @GUI_CFLAGS@

