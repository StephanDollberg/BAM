#!/bin/bash

name="test"

compiler="g++"
define_keyword="DEXTRA"
define_args="-D_GLIBCXX_USE_SCHED_YIELD -D_GLIBCXX_USE_NANOSLEEP"
compile_options="-std=c++0x -Wall -Wextra -Werror -pedantic-errors -pthread -O2 -o $name test_main.cpp"


if [ $# -eq 1 ]  && [ $1 == $define_keyword ]; then
echo "compiling:" 
echo $compiler $define_args $compile_options "..."
g++ $define_args $compile_options

else
echo "compiling:" 
echo $compiler $compile_options "..."
g++ $compile_options
fi

echo "running" 
echo "./"$name "..."
./$name
rm $name

