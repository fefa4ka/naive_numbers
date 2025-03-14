#pragma once

#include "vector.h"


#define MATRIX(matrix, row, column)                                            \
    VECTOR(matrix->number.values, (row) * ((matrix)->columns) + (column))
#define MATRIX_FOREACH(matrix)                                                 \
    for (size_t row = 0; row < matrix->rows; row++)                            \
        for (size_t column = 0; column < (matrix)->columns; column++)

inline number *number_from_matrix(matrix *A, size_t row, size_t column)
{
    return number_create(MATRIX(A, row, column));
}

matrix *matrix_create(size_t rows, size_t columns);
matrix *matrix_seed(matrix *instance, NN_TYPE default_value);
matrix *matrix_identity(size_t size, NN_TYPE default_value);
matrix *matrix_create_from_list(size_t rows, size_t columns, NN_TYPE *values);
matrix *matrix_from_vector(vector *v, size_t columns);
matrix *matrix_from_vectors(vector **vectors, size_t rows, size_t columns);
matrix *matrix_diagonal_from_vector(vector *v);
matrix *matrix_clone(matrix *original);
matrix *matrix_reshape(matrix *instance, size_t rows, size_t columns);

matrix *matrix_diagonal(matrix *A, size_t row, size_t column);
vector *matrix_column_vector(matrix *A, size_t column);
matrix *matrix_sub_matrix(matrix *A, size_t row, size_t column, size_t rows,
                          size_t columns);
matrix *matrix_minor_matrix(matrix *A, size_t exclude_row, size_t exclude_column);
matrix *matrix_transpose(matrix *instance);
vector *vector_transformation_by_matrix(matrix *A, vector *x);
matrix *matrix_multiplication(matrix *A, matrix *B);
matrix *matrix_map(matrix *A, NN_TYPE operation(NN_TYPE));
matrix *matrix_map_value(matrix *A, NN_TYPE operation(NN_TYPE, NN_TYPE *),
                         NN_TYPE *value);
int matrix_lu_decomposition(matrix *A, matrix **L, matrix **U);

NN_TYPE matrix_sum(matrix *A);
NN_TYPE matrix_trace(matrix *A);
NN_TYPE matrix_frobenius_norm(matrix *A);
NN_TYPE matrix_frobenius_norm_by_trace(matrix *A);
int   matrix_is_equal(matrix *A, matrix *B);
NN_TYPE matrix_determinant(matrix *A);

void matrix_print(matrix *instance);
#define MATRIX_CHECK_LOG(matrix, message, ...)                                 \
    {                                                                          \
        CHECK_MEMORY(matrix);                                                  \
        CHECK(matrix->number.type == NN_MATRIX, "Wrong matrix type. " message, \
              ##__VA_ARGS__);                                                  \
        CHECK((matrix)->columns && (matrix)->rows,                             \
              "Matrix size not set. " message, ##__VA_ARGS__);                 \
        VECTOR_CHECK_LOG((vector *)(matrix)->number.values,                    \
                         "Matrix values vector broken. " message,              \
                         ##__VA_ARGS__);                                       \
        CHECK((matrix)->number.values                                          \
                  && (matrix)->columns * (matrix)->rows                        \
                         == ((vector *)((matrix)->number.values))->length,     \
              "Matrix value broken %zdx%zd = %d. " message, (matrix)->rows,    \
              (matrix)->columns,                                               \
              ((matrix)->number.values                                         \
               && ((vector *)((matrix)->number.values))->length)               \
                  || 0,                                                        \
              ##__VA_ARGS__);                                                  \
    }
#define MATRIX_CHECK(matrix) MATRIX_CHECK_LOG(matrix, "")
