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
	-rm -f ui_* colourbutton.cpp gradientbar.cpp icons.cpp paneorganiser.cpp texponentialspin.cpp viewer.cpp 

libgui_a_SOURCES = icons.qrc texponentialspin.uih
libgui_a_SOURCES += texponentialspin.uih texponentialspin_funcs.cpp
libgui_a_SOURCES += axes.ui create.ui data.ui dataimport.ui equationselect.ui fitsetup.ui layout.ui log.ui referencedialog.ui saveimage.ui style.ui targetselect.ui transform.ui view.ui
libgui_a_SOURCES += uchroma.ui uchroma_collection.cpp uchroma_data.cpp uchroma_funcs.cpp uchroma_info.cpp uchroma_interaction.cpp uchroma_slots.cpp uchroma_view.cpp
libgui_a_SOURCES += io_load.cpp io_save.cpp keywords.cpp
libgui_a_SOURCES += axes_funcs.cpp create_funcs.cpp data_funcs.cpp dataimport_funcs.cpp dataimport_seqxy.cpp equationselect_funcs.cpp fitsetup_funcs.cpp layout_funcs.cpp log_funcs.cpp referencedialog_funcs.cpp saveimage_funcs.cpp style_funcs.cpp targetselect_funcs.cpp transform_funcs.cpp view_funcs.cpp
libgui_a_SOURCES += colourbutton.uih colourbutton_funcs.cpp
libgui_a_SOURCES += gradientbar.uih gradientbar_funcs.cpp
libgui_a_SOURCES += paneorganiser.uih paneorganiser_funcs.cpp
libgui_a_SOURCES += viewer.uih viewer_funcs.cpp viewer_input.cpp viewer_prefs.cpp viewer_user.cpp

noinst_HEADERS = uchroma.h axes.h create.h data.h dataimport.h equationselect.h fitsetup.h keywords.h layout.h log.h referencedialog.h saveimage.h style.h targetselect.h transform.h view.h

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @GUI_CFLAGS@

