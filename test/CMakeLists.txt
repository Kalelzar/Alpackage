cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
include(GoogleTest)

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS
        "*.cpp"
        )

set(RC_ENABLE_GTEST ON CACHE BOOL "Rapidcheck GTest Support" FORCE)
set(RC_ENABLE_GMOCK ON CACHE BOOL "Rapidcheck GMock Support" FORCE)
set(RC_ENABLE_BOOST ON CACHE BOOL "Rapidcheck Boost Support" FORCE)
set(RC_ENABLE_RTTI  ON CACHE BOOL "Rapidcheck RTTI Support"  FORCE)

add_executable(Alpackage_test ${TEST_SRC})

target_link_libraries(Alpackage_test PUBLIC $<$<CONFIG:ASAN>:asan> rapidcheck gtest gtest_main Alpackage dl)

gtest_discover_tests(Alpackage_test)

include(CodeCoverage)
append_coverage_compiler_flags(-fPIC -O1)
setup_target_for_coverage_fastcov(
  NAME TestCoverage
  EXECUTABLE ctest -j ${PROCESSOR_COUNT}
  DEPENDENCIES Alpackage_test
  BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
  EXCLUDE "lib" "/usr/lib" "/usr/local/lib" "/usr/include" "/usr/local/include")
