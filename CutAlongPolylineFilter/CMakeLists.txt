cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0020 NEW)

Project(CutAlongPolylineFilter)
#VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


#SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
SET(TEST_CUTALONGPOLYLINEFILTER_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/TestCutAlongPolyLineFilter.cxx)
SET(CUT_ALONG_POLYLINE_FILTER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/CutAlongPolyLineFilter.cxx)
SET(CUT_ALONG_POLYLINE_FILTER_INC ${CMAKE_CURRENT_SOURCE_DIR}/CutAlongPolyLineFilter.h)

add_library(CutAlongPolyLineFilter 	
	${CUT_ALONG_POLYLINE_FILTER_INC} 
	${CUT_ALONG_POLYLINE_FILTER_SRC}
	)

add_executable(Test_CutAlongPolyLineFilter 
	${CMAKE_CURRENT_SOURCE_DIR}/TestCutAlongPolyLineFilter.cxx
	)

target_link_libraries(CutAlongPolyLineFilter ${VTK_LIBRARIES})
target_link_libraries(Test_CutAlongPolyLineFilter  ${VTK_LIBRARIES} CutAlongPolyLineFilter)

#Grouping in MSVC
SOURCE_GROUP("Test"	FILES ${TEST_CUTALONGPOLYLINEFILTER_SRC})
SOURCE_GROUP("Files" 	FILES ${TEST_IO_MANAGER_INC}  ${TEST_IO_MANAGER_SRC} )
