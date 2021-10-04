cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(AlpackageTextDB VERSION 0.0.1)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo;Debug" CACHE STRING "Build type selections" FORCE)

include(CTest)
include(GoogleTest)


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system)

file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS
  "src/*.cpp"
  )

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS
  "test/*.cpp"
  )

add_library(AlpackageTextDB SHARED ${ALL_SRC})
set_target_properties(AlpackageTextDB PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(AlpackageTextDB PUBLIC Alpackage ${Boost_LIBRARIES} dl)
target_include_directories(AlpackageTextDB PUBLIC ${Boost_INCLUDE_DIRS})

add_executable(AlpackageTextDB_test ${TEST_SRC})
target_link_libraries(AlpackageTextDB_test PUBLIC rapidcheck gtest gtest_main AlpackageTextDB Alpackage  dl)

gtest_discover_tests(AlpackageTextDB_test)
