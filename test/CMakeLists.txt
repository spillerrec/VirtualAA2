cmake_minimum_required(VERSION 3.0)

include_directories ("${PROJECT_SOURCE_DIR}/../src")

add_executable(Tester main.cpp)
target_link_libraries(Tester VirtualAA2Core -lboost_unit_test_framework-mgw52-mt-1_61)

# Enable C++14 features
set_property(TARGET Tester PROPERTY CXX_STANDARD 14)
set_property(TARGET Tester PROPERTY CXX_STANDARD_REQUIRED ON)
