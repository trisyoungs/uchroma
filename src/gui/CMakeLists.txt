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
uchroma_funcs.cpp
uchroma_data.cpp
uchroma_analyse.cpp
uchroma_colour.cpp
uchroma_analyse.cpp
uchroma_sourcedata.cpp
uchroma_transform.cpp
uchroma_view.cpp
dataimport_funcs.cpp
dataimport_seqxy.cpp
saveimage_funcs.cpp
colourbutton_funcs.cpp
gradientbar_funcs.cpp
plotwidget_data.cpp
plotwidget_funcs.cpp
plotwidget_paintevent.cpp
viewer_funcs.cpp
viewer_input.cpp
viewer_prefs.cpp
viewer_primitive.cpp
viewer_primitiveinfo.cpp
viewer_primitivevertex.cpp
viewer_textprimitive.cpp
viewer_trianglechopper.cpp
viewer_vertexchunk.cpp
viewer_primitive.h
viewer_primitiveinfo.h
viewer_textprimitive.h
viewer_trianglechopper.h
viewer_user.cpp
viewer_vertexchunk.h
)

SET(pro_MOC_HDRS
  uchroma.h
  dataimport.h
  saveimage.h
  colourbutton.uih
  gradientbar.uih
  plotwidget.uih
  viewer.uih
)
QT4_WRAP_CPP(pro_MOC_SRCS ${pro_MOC_HDRS} OPTIONS -f)

SET(pro_UIS
  dataimport.ui
  saveimage.ui
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

