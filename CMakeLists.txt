cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXTRA_CXXFLAGS "-Wall -Wextra")

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common application)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(WalkieTalkie)