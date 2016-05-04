cmake_minimum_required(VERSION 3.0)

# C++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Turn on warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCES_DECODERS
	decoders/PPArchive.cpp
	decoders/XXModel.cpp
	)

set(SOURCES_UTILS
	utils/ArrayView.cpp
	)

# add_executable(testing ${SOURCES_DECODERS} ${SOURCES_UTILS} main.cpp)

ADD_DEFINITIONS(-DUNICODE)
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -municode" )

add_executable(VirtualAA2 ${SOURCES_DECODERS} ${SOURCES_UTILS} main-dokan.cpp)
target_link_libraries(VirtualAA2 -ldokan1)
