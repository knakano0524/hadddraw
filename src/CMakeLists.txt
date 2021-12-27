cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(haddraw CXX)

file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.h)

find_package(Boost COMPONENTS filesystem)
if(Boost_FOUND)
  message("-- Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
  message("-- Boost_LIBRARIES    = ${Boost_LIBRARIES}")
  include_directories(${Boost_INCLUDE_DIRS})
endif()

#find_package(ROOT REQUIRED)
#message("ROOT_INCLUDE_DIRS: ${ROOT_INCLUDE_DIRS}")
#message("ROOT_LIBRARIES: ${ROOT_LIBRARIES}")
#message("ROOT_CXX_FLAGS: ${ROOT_CXX_FLAGS}")

find_program(ROOTCONF "root-config")
if(ROOTCONF)
  message("-- Detecting ROOT:    found at ${ROOTCONF}")
else()
  message(FATAL_ERROR "-- Detecting ROOT:    not found")
endif()
execute_process(COMMAND root-config --prefix OUTPUT_VARIABLE ROOT_PREFIX  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs   OUTPUT_VARIABLE ROOT_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libdir OUTPUT_VARIABLE ROOT_LIBDIR  OUTPUT_STRIP_TRAILING_WHITESPACE)
#message("ROOT_PREFIX: ${ROOT_PREFIX}")
#message("ROOT_CFLAGS: ${ROOT_CFLAGS}")
#message("ROOT_LINK  : ${ROOT_LINK}")
#message("ROOT_LIBDIR: ${ROOT_LIBDIR}")

set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}         ${ROOT_CFLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  ${ROOT_LINK}")
list( APPEND CMAKE_INSTALL_RPATH ${ROOT_LIBDIR} )

add_executable(hadddraw
	${PROJECT_SOURCE_DIR}/hadddraw.cc
)


target_link_libraries(hadddraw
	${Boost_LIBRARIES} ${ROOT_LINK}
)

install(
	TARGETS hadddraw
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)
