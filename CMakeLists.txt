cmake_minimum_required (VERSION 2.6)
project (bam)

set (CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -g -fno-omit-frame-pointer -std=c++11")

add_subdirectory(test)

enable_testing()
