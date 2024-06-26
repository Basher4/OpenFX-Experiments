cmake_minimum_required(VERSION 3.28)
project(openfx_experiments)

add_library(basic SHARED basic.cpp)
add_library(invert SHARED invert.cpp)
add_library(rectangle SHARED rectangle.cpp)
