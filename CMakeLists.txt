cmake_minimum_required(VERSION 3.1)

project(nnn VERSION 1.0
    DESCRIPTION "Naive Numbers"
    LANGUAGES C)

find_package(OpenMP)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_library(nn_number STATIC src/number.c src/utils.c)
target_include_directories(nn_number PUBLIC include)

add_library(nn_vector STATIC src/vector.c)
target_link_libraries(nn_vector nn_number)

add_library(nn_text STATIC src/text.c)
target_link_libraries(nn_text nn_number)

add_library(nn_matrix STATIC src/matrix.c)
target_link_libraries(nn_matrix nn_vector)

add_library(nn_probability STATIC src/probability.c)
target_link_libraries(nn_probability nn_matrix nn_vector)

enable_testing()

add_executable(test_vector test/vector.c)
target_link_libraries(test_vector nn_vector)

add_executable(test_matrix test/matrix.c)
target_link_libraries(test_matrix nn_matrix)

add_executable(test_probability test/probability.c)
target_link_libraries(test_probability nn_probability)

add_test(NAME vector
    COMMAND test_vector)
add_test(NAME matrix
    COMMAND test_matrix)
add_test(NAME probability
    COMMAND test_probability)

