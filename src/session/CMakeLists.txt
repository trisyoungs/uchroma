add_library(session
  editstate.cpp
  editstate_axes.cpp
  editstate_collection.cpp
  editstate_viewpane.cpp
  editstatedata.cpp
  editstategroup.cpp
  keywords.cpp
  load.cpp
  save.cpp
  session.cpp
  editstate.h
  editstatedata.h
  editstategroup.h
  session.h
)

target_include_directories(session PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)
