cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(Alpackage VERSION 0.0.1)

file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS
        "*.cpp"
        )

set (BOOST_ROOT "${CMAKE_SOURCE_DIR}/lib/boost")
set (BOOST_INCLUDEDIR "${CMAKE_SOURCE_DIR}/lib/boost/")
set (BOOST_LIBRARYDIR "${CMAKE_SOURCE_DIR}/lib/boost/stage/lib")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.76 REQUIRED COMPONENTS system filesystem)

include_directories(${CMAKE_SOURCE_DIR}/lib/reckless/reckless/include)

add_library(Alpackage SHARED ${ALL_SRC})
target_include_directories(Alpackage PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(Alpackage $<$<CONFIG:ASAN>:asan> $<$<CONFIG:Debug>:profiler> $<$<CONFIG:Release>:tcmalloc> $<$<CONFIG:Debug>:tcmalloc> ${Boost_LIBRARIES} dl)
target_link_libraries(Alpackage reckless pthread)
set_target_properties(Alpackage PROPERTIES LINKER_LANGUAGE CXX)
