cmake_minimum_required(VERSION 3.1)

project(nnn VERSION 1.0
    DESCRIPTION "Naive Numbers"
    LANGUAGES C)

find_package(OpenMP)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_library(nn_number STATIC src/number.c)
target_include_directories(nn_number PUBLIC include)

add_library(nn_vector STATIC src/vector.c)
target_link_libraries(nn_vector nn_number)

# https://github.com/SixTrack/SixTrack/blob/master/CMakeLists.txt
# set(CMAKE_C_FLAGS "-mavx -mavx2 ${CMAKE_C_FLAGS}")

add_executable(test_basic test/basic.c)
target_link_libraries(test_basic nn_vector)

add_executable(test_vector_addition test/vector_addition.c)
target_link_libraries(test_vector_addition nn_vector)

add_test(NAME test_basic
    COMMAND test_basic)

