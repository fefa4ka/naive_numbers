cmake_minimum_required(VERSION 3.1)

project(nnn VERSION 1.0
    DESCRIPTION "Naive Numbers"
    LANGUAGES C)

# Set the build type to Debug to include debug symbols
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add debug flags for the compiler
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

OPTION (USE_OpenMP "Use OpenMP to enable <omp.h>" ON)

if(APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")

    if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_C_LIB_NAMES "omp")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_CXX_LIB_NAMES "omp")
    endif()

endif()

find_package(OpenMP REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/simde)


add_library(nn_number STATIC src/number.c src/utils.c)
target_include_directories(nn_number PUBLIC include)

add_library(nn_vector STATIC src/vector.c)
target_link_libraries(nn_vector nn_number OpenMP::OpenMP_C)

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

# Chained operations test
add_executable(test_chained_operations test/chained_operations.c)
target_link_libraries(test_chained_operations nn_probability)
add_test(NAME chained_operations
    COMMAND test_chained_operations)
