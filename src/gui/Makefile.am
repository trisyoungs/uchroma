noinst_LIBRARIES = libgui.a

SUFFIXES: .ui .hui .qrc

.ui.o:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/$*.h
	$(CXX) -I$(top_srcdir)/src -I$(top_srcdir)/src/gui/ $(CXXFLAGS) @UCHROMA_CFLAGS@ -c -o $@ moc_$*.cpp
	rm moc_$*.cpp

.hui.o:
	$(QTMOC) -o $*.cpp -f$< $<
	$(CXX) -I$(top_srcdir)/src -I../ -I./ $(CXXFLAGS) @UCHROMA_CFLAGS@ -c -o $@ $*.cpp
	rm $*.cpp

.qrc.o:
	$(QTRCC) -o $*.cpp -name $* $<
	$(CXX) -I$(top_srcdir)/src -I../ -I./ $(CXXFLAGS) $(QTUCHROMA_CFLAGS) -c -o $@ $*.cpp
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o colourbutton.cpp gradientbar.cpp icons.cpp paneorganiser.cpp texponentialspin.cpp viewer.cpp 

libgui_a_SOURCES = icons.qrc texponentialspin.hui

libgui_a_SOURCES += editdataset.ui editfitkernel.ui editfitresults.ui editlinestyle.ui editnumberformat.ui editreference.ui editviewlayout.ui
libgui_a_SOURCES += selectequation.ui selectsymbol.ui selecttarget.ui

libgui_a_SOURCES += axes.ui create.ui data.ui import.ui log.ui saveimage.ui style.ui transform.ui view.ui
libgui_a_SOURCES += operate_bgsub.ui operate_setz.ui
libgui_a_SOURCES += uchroma.ui

libgui_a_SOURCES += uchroma_collection.cpp uchroma_funcs.cpp uchroma_info.cpp uchroma_interaction.cpp uchroma_slots.cpp
libgui_a_SOURCES += axes_funcs.cpp create_funcs.cpp data_funcs.cpp import_funcs.cpp import_seqxy.cpp log_funcs.cpp saveimage_funcs.cpp style_funcs.cpp transform_funcs.cpp view_funcs.cpp
libgui_a_SOURCES += editdataset_funcs.cpp editfitkernel_funcs.cpp editfitresults_funcs.cpp editlinestyle_funcs.cpp editnumberformat_funcs.cpp editreference_funcs.cpp editviewlayout_funcs.cpp
libgui_a_SOURCES += operate_bgsub_funcs.cpp operate_setz_funcs.cpp
libgui_a_SOURCES += selectequation_funcs.cpp selectsymbol_funcs.cpp selecttarget_funcs.cpp

libgui_a_SOURCES += texponentialspin.hui texponentialspin_funcs.cpp
libgui_a_SOURCES += colourbutton.hui colourbutton_funcs.cpp
libgui_a_SOURCES += gradientbar.hui gradientbar_funcs.cpp
libgui_a_SOURCES += paneorganiser.hui paneorganiser_funcs.cpp

libgui_a_SOURCES += viewer.hui viewer_funcs.cpp viewer_input.cpp

noinst_HEADERS = uchroma.h axes.h create.h data.h import.h log.h saveimage.h style.h transform.h view.h
noinst_HEADERS += editdataset.h editfitkernel.h editfitresults.h editlinestyle.h editnumberformat.h editreference.h editviewlayout.h
noinst_HEADERS += operate_bgsub.h operate_setz.h
noinst_HEADERS += selectequation.h selectsymbol.h selecttarget.h

libgui_a_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @UCHROMA_CFLAGS@

