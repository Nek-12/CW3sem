cmake_minimum_required(VERSION 3.17)
project(CW3 LANGUAGES CXX)

# set( CMAKE_VERBOSE_MAKEFILE ON )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_CXX_FLAGS_MINSIZEREL is ${CMAKE_CXX_FLAGS_MINSIZEREL}")

set(GCC_COVERAGE_COMPILE_FLAGS "-unicode -Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings \
-Wlogical-op -Wredundant-decls -Woverloaded-virtual -Wno-nonnull-compare -Wno-address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# Disable building tests and examples in libfort project
set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")


add_executable(CW3 lib/sha256.h lib/sha256.cpp src/App.cpp src/Entry.hpp src/Entry.cpp src/CONST.hpp src/util.hpp src/util.cpp src/Journal.hpp src/Data.cpp src/Data.hpp src/UI.hpp src/DateTime.h src/DateTime.cpp src/App.hpp)
add_subdirectory(lib/libfort)
target_link_libraries(CW3 PRIVATE fort)
