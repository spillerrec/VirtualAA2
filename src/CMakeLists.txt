cmake_minimum_required(VERSION 3.0)

# C++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Qt5Gui REQUIRED)

# Turn on warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCES_CORE
	PPArchive.cpp
	XXModel.cpp
	main.cpp
	)


add_executable(VirtualAA2 ${SOURCES_CORE} )
target_link_libraries(VirtualAA2 Qt5::Gui)
