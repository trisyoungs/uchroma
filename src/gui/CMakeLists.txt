include_directories(
./
../
${CMAKE_CURRENT_BINARY_DIR} 
${CMAKE_CURRENT_BINARY_DIR}/../
${QT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/src/gui 
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src/
)

# Source files (not Qt UI files or custom .h [uih])
SET(qtproject_SRCS
uchroma_collection.cpp
uchroma_data.cpp
uchroma_funcs.cpp
uchroma_info.cpp
uchroma_interaction.cpp
uchroma_slots.cpp
uchroma_view.cpp
io_load.cpp
io_save.cpp
keywords.cpp
axes_funcs.cpp
create_funcs.cpp
data_funcs.cpp
dataimport_funcs.cpp
dataimport_seqxy.cpp
saveimage_funcs.cpp
colourbutton_funcs.cpp
equationselect_funcs.cpp
fit_funcs.cpp
fit_main.cpp
fit_sd.cpp
fit_simplex.cpp
fit_target.cpp
gradientbar_funcs.cpp
graphwidget_funcs.cpp
plotwidget_data.cpp
plotwidget_funcs.cpp
plotwidget_paintevent.cpp
slicemonitor_funcs.cpp
style_funcs.cpp
texponentialspin_funcs.cpp
transform_funcs.cpp 
uchroma_data.cpp
view_funcs.cpp
viewer_funcs.cpp
viewer_input.cpp
viewer_prefs.cpp
viewer_user.cpp
)

SET(pro_MOC_HDRS
  axes.h
  uchroma.h
  create.h
  data.h
  dataimport.h
  equationselect.h
  fit.h
  graphwidget.h
  saveimage.h
  slicemonitor.h
  style.h
  transform.h
  view.h
  colourbutton.uih
  gradientbar.uih
  plotwidget.uih
  texponentialspin.uih
  viewer.uih
)
QT4_WRAP_CPP(pro_MOC_SRCS ${pro_MOC_HDRS} OPTIONS -f)

SET(pro_UIS
  axes.ui
  create.ui
  data.ui
  dataimport.ui
  equationselect.ui
  fit.ui
  graphwidget.ui
  saveimage.ui
  slicemonitor.ui
  style.ui
  transform.ui
  view.ui
  uchroma.ui
)
QT4_WRAP_UI(pro_UIS_H ${pro_UIS})

set(pro_RES_QRC
  icons.qrc 
)
QT4_ADD_RESOURCES(pro_RES ${pro_RES_QRC})

add_library(gui
${pro_RES}
${pro_UIS_H}
${qtproject_SRCS}
${pro_MOC_SRCS}
)

