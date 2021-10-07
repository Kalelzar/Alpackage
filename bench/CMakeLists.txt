cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

include(CTest)

file(GLOB_RECURSE BENCH_SRC CONFIGURE_DEPENDS
        "*.cpp"
        )

add_executable(Alpackage_bench ${BENCH_SRC})
target_link_libraries(Alpackage_bench PUBLIC $<$<CONFIG:ASAN>:asan> hayai_main Alpackage)
