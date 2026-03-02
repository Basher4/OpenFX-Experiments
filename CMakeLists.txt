cmake_minimum_required(VERSION 4.1)
project(openfx_experiments)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

add_subdirectory(plugins)

add_executable(openfx_experiments main.cpp
        src/imageIo.cpp
        src/host.cpp
        src/memorySuite.cpp
        src/propertySet.cpp
)

if (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    target_link_libraries(${PROJECT_NAME} stdc++exp) # std:: print, println, ...
endif()

find_package(OpenCV CONFIG REQUIRED)
target_link_libraries(openfx_experiments ${OpenCV_LIBS})
#target_link_libraries(openfx_experiments PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)
