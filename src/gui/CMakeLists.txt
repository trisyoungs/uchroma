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
  editdataset_funcs.cpp
  editfitkernel_funcs.cpp
  editlinestyle_funcs.cpp
  editnumberformat_funcs.cpp
  editreference_funcs.cpp
  editviewlayout_funcs.cpp

  selectequation_funcs.cpp
  selectsymbol_funcs.cpp
  selecttarget_funcs.cpp

  colourbutton_funcs.cpp
  gradientbar_funcs.cpp
  paneorganiser_funcs.cpp
  texponentialspin_funcs.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_prefs.cpp
  viewer_user.cpp

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
  data_setz_funcs.cpp
  dataimport_funcs.cpp
  dataimport_seqxy.cpp
  log_funcs.cpp
  saveimage_funcs.cpp
  style_funcs.cpp
  transform_funcs.cpp 
  view_funcs.cpp
)

SET(pro_MOC_HDRS
  colourbutton.uih
  gradientbar.uih
  paneorganiser.uih
  texponentialspin.uih
  viewer.uih

  editdataset.h
  editfitkernel.h
  editlinestyle.h
  editnumberformat.h
  editreference.h
  editviewlayout.h

  selectequation.h
  selectsymbol.h
  selecttarget.h

  axes.h
  uchroma.h
  create.h
  data.h
  data_setz.h
  dataimport.h
  log.h
  saveimage.h
  style.h
  transform.h
  view.h
)
QT4_WRAP_CPP(pro_MOC_SRCS ${pro_MOC_HDRS} OPTIONS -f)

SET(pro_UIS
  editdataset.ui
  editfitkernel.ui
  editlinestyle.ui
  editnumberformat.ui
  editreference.ui
  editviewlayout.ui

  selectequation.ui
  selectsymbol.ui
  selecttarget.ui

  axes.ui
  create.ui
  data.ui
  data_setz.ui
  dataimport.ui
  log.ui
  saveimage.ui
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

