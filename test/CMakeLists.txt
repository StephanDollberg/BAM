include_directories(${TEST_SOURCE_DIR}/..)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

add_executable(bam_test 
    test_runner.cpp 
    async_test.cpp
    parallel_copy_test.cpp
    parallel_find_test.cpp
    parallel_for_each_test.cpp
    parallel_for_test.cpp
    parallel_invoke_test.cpp
    parallel_reduce_test.cpp
    parallel_transform_test.cpp
    task_pool_test.cpp
    timer_test.cpp
)

enable_testing()
add_test(test bam_test)
