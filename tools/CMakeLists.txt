cmake_minimum_required(VERSION 3.0)

# Turn on warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

ADD_DEFINITIONS(-DUNICODE)
include_directories(../src/)

configure_file(xxViewer/vertex.glsl   ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(xxViewer/fragment.glsl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(XXViewer xxViewer/main.cpp)
target_link_libraries(XXViewer VirtualAA2Core -lopengl32 -lglfw3dll -lglew32)

# Enable C++14 features
set_property(TARGET XXViewer PROPERTY CXX_STANDARD 14)
set_property(TARGET XXViewer PROPERTY CXX_STANDARD_REQUIRED ON)
