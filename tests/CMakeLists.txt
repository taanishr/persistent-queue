# In tests/CMakeLists.txt
add_executable(pqueue_tests pqueue_tests.cpp)

# Link the test executable with Catch2 and main executable (pqueue_exec)
target_link_libraries(pqueue_tests PRIVATE Catch2::Catch2WithMain)

target_include_directories(pqueue_tests PRIVATE ${CMAKE_SOURCE_DIR})