cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
include(GoogleTest)

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS
        "*.cpp"
        )

add_executable(Alpackage_test ${TEST_SRC})
target_link_libraries(Alpackage_test PUBLIC rapidcheck gtest gtest_main Alpackage dl)

gtest_discover_tests(Alpackage_test)
