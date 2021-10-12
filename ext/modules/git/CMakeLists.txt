cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(AlpackageGit VERSION 0.0.1)

set(CMAKE_CONFIGURATION_TYPES "Release;ASAN;RelWithDebInfo;Debug" CACHE STRING "Build type selections" FORCE)

include(CTest)
include(GoogleTest)

set (BOOST_ROOT "${CMAKE_SOURCE_DIR}/lib/boost")
set (BOOST_INCLUDEDIR "${CMAKE_SOURCE_DIR}/lib/boost/")
set (BOOST_LIBRARYDIR "${CMAKE_SOURCE_DIR}/lib/boost/stage/lib")
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

add_library(AlpackageGit SHARED ${ALL_SRC})
set_target_properties(AlpackageGit PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(AlpackageGit PUBLIC git2 Alpackage ${Boost_LIBRARIES} dl)
target_include_directories(AlpackageGit PUBLIC ${Boost_INCLUDE_DIRS})

set(RC_ENABLE_GTEST ON CACHE BOOL "Rapidcheck GTest Support" FORCE)
set(RC_ENABLE_GMOCK ON CACHE BOOL "Rapidcheck GMock Support" FORCE)
set(RC_ENABLE_BOOST ON CACHE BOOL "Rapidcheck Boost Support" FORCE)
set(RC_ENABLE_RTTI  ON CACHE BOOL "Rapidcheck RTTI Support"  FORCE)

add_executable(AlpackageGit_test ${TEST_SRC})
target_link_libraries(AlpackageGit_test PUBLIC $<$<CONFIG:ASAN>:asan> rapidcheck gtest gtest_main git2 AlpackageGit Alpackage dl)


gtest_discover_tests(AlpackageGit_test)
