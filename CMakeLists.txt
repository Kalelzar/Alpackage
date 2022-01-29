cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(Alpackage VERSION 0.0.1)

set(CMAKE_CONFIGURATION_TYPES "Release;ASAN;Debug" CACHE STRING "Build type selections" FORCE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(CodeCoverage)
append_coverage_compiler_flags(-fPIC -O0 --coverage -fkeep-inline-functions -fkeep-static-functions)

include(CTest)

# Libs

set (BOOST_ROOT "lib/boost")

add_compile_options(-fPIC $<$<CONFIG:ASAN>:-fsanitize=address> -fconcepts-diagnostics-depth=2)


include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(lib/reckless) # For logging
include_directories(${CMAKE_SOURCE_DIR}/lib/reckless/reckless/include)

add_subdirectory(src)
add_subdirectory(ext)

add_executable(Alpackage_main exec/main.cpp)
set_target_properties(Alpackage_main PROPERTIES OUTPUT_NAME "alpkg")
target_link_libraries(Alpackage_main PUBLIC Alpackage dl)
#target_link_libraries(Alpackage_main PUBLIC reckless pthread)

add_subdirectory(test)
add_subdirectory(bench)

install(TARGETS Alpackage_main Alpackage
  RUNTIME DESTINATION
  LIBRARY DESTINATION)
