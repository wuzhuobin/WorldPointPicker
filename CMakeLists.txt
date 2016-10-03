
cmake_minimum_required(VERSION 2.8)

PROJECT(vtkInteractorStyle3dContourTest)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(
  MAIN_DIRECTORY
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/CutAlongPolylineFilter
  ${PROJECT_BINARY_DIR}/CutAlongPolylineFilter
)
include_directories(
  ${MAIN_DIRECTORY}
)

set( 
  WP_SRC
  WorldPointPicker.cxx
  AbstractInteractorStyle.cxx
  AbstractInteractorStyle3D.cxx
  vtkInteractorStyle3dContour.cpp
  vtkInteractorStyle3dContour2.cpp
)

set(
  WP_INC
  AbstractInteractorStyle.h
  AbstractInteractorStyle3D.h
  vtkInteractorStyle3dContour.h
  vtkInteractorStyle3dContour2.h
  
)

add_subdirectory(CutAlongPolylineFilter)

add_executable(vtkInteractorStyle3dContourTest 
  MACOSX_BUNDLE
  ${WP_INC}
  ${WP_SRC}
  )

if(VTK_LIBRARIES)
  target_link_libraries(vtkInteractorStyle3dContourTest ${VTK_LIBRARIES})
else()
  target_link_libraries(vtkInteractorStyle3dContourTest vtkHybrid vtkWidgets)
endif()

target_link_libraries(
  vtkInteractorStyle3dContourTest
  CutAlongPolyLineFilter
)