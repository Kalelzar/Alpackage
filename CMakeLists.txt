cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(Alpackage VERSION 0.0.1)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo;Debug" CACHE STRING "Build type selections" FORCE)

include(CTest)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(ext)


add_executable(Alpackage_main exec/main.cpp)
target_link_libraries(Alpackage_main PUBLIC Alpackage dl)

add_subdirectory(test)
add_subdirectory(bench)
