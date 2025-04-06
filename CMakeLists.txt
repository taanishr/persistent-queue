cmake_minimum_required(VERSION 3.10)
project(pqueue)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

add_executable(pqueue main.cpp pqueue/pqueue.cpp pqueue/pqueue_engine.cpp)

target_link_libraries(pqueue PRIVATE Catch2::Catch2)

target_include_directories(pqueue PRIVATE ${CMAKE_SOURCE_DIR})

add_subdirectory(tests)
