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
log_funcs.cpp
referencedialog_funcs.cpp
saveimage_funcs.cpp
colourbutton_funcs.cpp
equationselect_funcs.cpp
fitsetup_funcs.cpp
gradientbar_funcs.cpp
layout_funcs.cpp
paneorganiser_funcs.cpp
style_funcs.cpp
targetselect_funcs.cpp
texponentialspin_funcs.cpp
transform_funcs.cpp 
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
  fitsetup.h
  layout.h
  log.h
  referencedialog.h
  saveimage.h
  style.h
  targetselect.h
  transform.h
  view.h
  colourbutton.uih
  gradientbar.uih
  paneorganiser.uih
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
  fitsetup.ui
  log.ui
  referencedialog.ui
  saveimage.ui
  style.ui
  targetselect.ui
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

