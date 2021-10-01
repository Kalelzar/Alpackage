cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(Alpackage VERSION 0.0.1)

file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS
        "*.cpp"
        )

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system filesystem)


add_library(Alpackage SHARED ${ALL_SRC})
target_include_directories(Alpackage PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(Alpackage ${Boost_LIBRARIES} dl)
set_target_properties(Alpackage PROPERTIES LINKER_LANGUAGE CXX)
