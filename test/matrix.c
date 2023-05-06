/* https://mathworld.wolfram.com/MatrixTrace.html */

#include "matrix.h"
#include "vector.h"
#include <math.h>
#include <nn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define log_print(type, message, ...)                                          \
    printf(type "\t" message "\n", ##__VA_ARGS__)
#define log_debug(type, message, ...)                                          \
    log_print(type, message " (%s:%d)\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define log_verbose(message, ...) log_print("VERBOSE", message, ##__VA_ARGS__)
#define log_info(message, ...)    log_print("INFO", message, ##__VA_ARGS__)
#define log_ok(message, ...)      log_print("OK", message, ##__VA_ARGS__)
#define log_error(message, ...)                                                \
    log_print("\e[1m\e[31mERROR\e[39m\e[0m", message " (%s:%d)\n",             \
              ##__VA_ARGS__, __FILE__, __LINE__)

#define test_assert(test, message, ...)                                        \
    if (!(test)) {                                                             \
        log_error(message, ##__VA_ARGS__);                                     \
        return 1;                                                              \
    } else {                                                                   \
        log_ok(message, ##__VA_ARGS__);                                        \
    }


int test_matrix_create()
{
    int     rows = 3;
    int     cols = 4;
    matrix *m    = matrix_create(rows, cols);
    test_assert(m, "Matrix allocated");
    test_assert(m->rows == rows, "Matrix rows initialized");
    test_assert(m->columns == cols, "Matrix columns initialized");

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    return 0;
}

int test_matrix_seed()
{
    int     rows = 3;
    int     cols = 4;
    matrix *m    = matrix_create(rows, cols);
    test_assert(m, "Matrix allocated");

    NN_TYPE default_value = 1.0;
    matrix_seed(m, default_value);

    MATRIX_FOREACH(m)
    {
        test_assert(MATRIX(m, row, column) == default_value,
                    "Matrix seeded with default value");
    }

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    return 0;
}

int test_matrix_identity()
{
    int     size = 3;
    matrix *m    = matrix_identity(size);
    test_assert(m, "Identity matrix allocated");
    test_assert(m->rows == size, "Identity matrix rows initialized");
    test_assert(m->columns == size, "Identity matrix columns initialized");

    MATRIX_FOREACH(m)
    {
        if (row == column) {
            test_assert(MATRIX(m, row, column) == 1.0,
                        "Identity matrix diagonal element is 1");
        } else {
            test_assert(MATRIX(m, row, column) == 0.0,
                        "Identity matrix off-diagonal element is 0");
        }
    }

    int r = number_delete(m);
    test_assert(r == 0, "Identity matrix deleted");

    return 0;
}

int test_matrix_create_from_list()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *m        = matrix_create_from_list(rows, cols, values);
    test_assert(m, "Matrix allocated");
    test_assert(m->rows == rows, "Matrix rows initialized");
    test_assert(m->columns == cols, "Matrix columns initialized");

    MATRIX_FOREACH(m)
    {
        test_assert(MATRIX(m, row, column) == values[row * cols + column],
                    "Matrix element initialized from list");
    }

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    return 0;
}

int test_matrix_from_vector()
{
    int     rows     = 3;
    int     cols     = 1;
    NN_TYPE values[] = {1.0, 2.0, 3.0};
    vector *v        = vector_from_list(rows, values);
    test_assert(v, "Vector allocated");

    matrix *m = matrix_from_vector(v, cols);
    test_assert(m, "Matrix allocated");
    test_assert(m->rows == rows, "Matrix rows initialized");
    test_assert(m->columns == cols, "Matrix columns initialized");

    MATRIX_FOREACH(m)
    {
        test_assert(MATRIX(m, row, column) == values[row],
                    "Matrix element initialized from vector");
    }

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    r = number_delete(v);
    test_assert(r == 0, "Vector deleted");

    return 0;
}

int test_matrix_from_vectors()
{
    int     rows      = 2;
    int     cols      = 3;
    NN_TYPE values1[] = {1.0, 2.0, 3.0};
    NN_TYPE values2[] = {4.0, 5.0, 6.0};
    vector *v1        = vector_from_list(rows, values1);
    vector *v2        = vector_from_list(rows, values2);
    vector *vectors[] = {v1, v2};

    matrix *m = matrix_from_vectors(vectors, rows, cols);
    test_assert(m, "Matrix allocated");
    test_assert(m->rows == rows, "Matrix rows initialized");
    test_assert(m->columns == cols, "Matrix columns initialized");

    MATRIX_FOREACH(m)
    {
        test_assert(MATRIX(m, row, column) == VECTOR(vectors[row], column),
                    "Matrix element initialized from vectors");
    }

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    r = number_delete(v1);
    test_assert(r == 0, "Vector 1 deleted");

    r = number_delete(v2);
    test_assert(r == 0, "Vector 2 deleted");

    return 0;
}

int test_matrix_diagonal_from_vector()
{
    int     size     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0};
    vector *v        = vector_from_list(size, values);
    test_assert(v, "Vector allocated");

    matrix *m = matrix_diagonal_from_vector(v);
    test_assert(m, "Matrix allocated");
    test_assert(m->rows == size, "Matrix rows initialized");
    test_assert(m->columns == size, "Matrix columns initialized");

    MATRIX_FOREACH(m)
    {
        if (row == column) {
            test_assert(MATRIX(m, row, column) == values[row],
                        "Matrix diagonal element initialized from vector");
        } else {
            test_assert(MATRIX(m, row, column) == 0.0,
                        "Matrix off-diagonal element is 0");
        }
    }

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    r = number_delete(v);
    test_assert(r == 0, "Vector deleted");

    return 0;
}

int test_matrix_clone()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *original = matrix_create_from_list(rows, cols, values);
    test_assert(original, "Original matrix allocated");
    test_assert(original->rows == rows, "Original matrix rows initialized");
    test_assert(original->columns == cols,
                "Original matrix columns initialized");

    matrix *clone = matrix_clone(original);
    test_assert(clone, "Cloned matrix allocated");
    test_assert(clone->rows == rows, "Cloned matrix rows initialized");
    test_assert(clone->columns == cols, "Cloned matrix columns initialized");

    MATRIX_FOREACH(clone)
    {
        test_assert(MATRIX(clone, row, column) == MATRIX(original, row, column),
                    "Cloned matrix element equals original matrix element");
    }

    int r = number_delete(original);
    test_assert(r == 0, "Original matrix deleted");

    r = number_delete(clone);
    test_assert(r == 0, "Cloned matrix deleted");

    return 0;
}

int test_matrix_reshape()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *original = matrix_create_from_list(rows, cols, values);
    test_assert(original, "Original matrix allocated");
    test_assert(original->rows == rows, "Original matrix rows initialized");
    test_assert(original->columns == cols,
                "Original matrix columns initialized");

    int     new_rows = 3;
    int     new_cols = 2;
    matrix *reshaped = matrix_reshape(original, new_rows, new_cols);
    test_assert(reshaped, "Reshaped matrix allocated");
    test_assert(reshaped->rows == new_rows, "Reshaped matrix rows initialized");
    test_assert(reshaped->columns == new_cols,
                "Reshaped matrix columns initialized");

    int index = 0;
    MATRIX_FOREACH(original)
    {
        test_assert(MATRIX(original, row, column)
                        == MATRIX(reshaped, index / new_cols, index % new_cols),
                    "Reshaped matrix element equals original matrix element");
        index++;
    }

    int r = number_delete(original);
    test_assert(r == 0, "Matrix deleted");

    return 0;
}

int test_matrix_transpose()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *original = matrix_create_from_list(rows, cols, values);
    test_assert(original, "Original matrix allocated");
    test_assert(original->rows == rows, "Original matrix rows initialized");
    test_assert(original->columns == cols,
                "Original matrix columns initialized");

    matrix *transposed = matrix_transpose(original);
    test_assert(transposed, "Transposed matrix allocated");
    test_assert(transposed->rows == cols, "Transposed matrix rows initialized");
    test_assert(transposed->columns == rows,
                "Transposed matrix columns initialized");

    MATRIX_FOREACH(original)
    {
        test_assert(MATRIX(original, row, column)
                        == MATRIX(transposed, column, row),
                    "Transposed matrix element equals original matrix element");
    }

    int r = number_delete(transposed);
    test_assert(r == 0, "Matrix deleted");

    return 0;
}

int test_vector_transformation_by_matrix()
{
    int     rows      = 2;
    int     cols      = 3;
    NN_TYPE values1[] = {1.0, 2.0, 3.0};
    NN_TYPE values2[] = {4.0, 5.0, 6.0};
    vector *v1        = vector_from_list(rows, values1);
    vector *v2        = vector_from_list(rows, values2);
    vector *vectors[] = {v1, v2};

    matrix *m = matrix_from_vectors(vectors, rows, cols);
    test_assert(m, "Matrix allocated");
    test_assert(m->rows == rows, "Matrix rows initialized");
    test_assert(m->columns == cols, "Matrix columns initialized");

    NN_TYPE values3[] = {1.0, 2.0, 3.0};
    vector *v3        = vector_from_list(cols, values3);

    vector *result = vector_transformation_by_matrix(m, v3);
    test_assert(result, "Result vector allocated");
    test_assert(result->length == rows, "Result vector size initialized");

    for (int i = 0; i < rows; i++) {
        NN_TYPE expected = 0.0;
        for (int j = 0; j < cols; j++) {
            expected += values3[j] * MATRIX(m, i, j);
        }
        test_assert(VECTOR(result, i) == expected,
                    "Result vector element equals expected value");
    }

    int r = number_delete(m);
    test_assert(r == 0, "Matrix deleted");

    r = number_delete(v1);
    test_assert(r == 0, "Vector 1 deleted");

    r = number_delete(v2);
    test_assert(r == 0, "Vector 2 deleted");

    r = number_delete(v3);
    test_assert(r == 0, "Vector 3 deleted");

    r = number_delete(result);
    test_assert(r == 0, "Result vector deleted");

    return 0;
}

int test_matrix_multiplication()
{
    int     rows_A     = 2;
    int     cols_A     = 3;
    NN_TYPE values_A[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *A          = matrix_create_from_list(rows_A, cols_A, values_A);
    test_assert(A, "Matrix A allocated");
    test_assert(A->rows == rows_A, "Matrix A rows initialized");
    test_assert(A->columns == cols_A, "Matrix A columns initialized");

    int     rows_B     = 3;
    int     cols_B     = 2;
    NN_TYPE values_B[] = {7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    matrix *B          = matrix_create_from_list(rows_B, cols_B, values_B);
    test_assert(B, "Matrix B allocated");
    test_assert(B->rows == rows_B, "Matrix B rows initialized");
    test_assert(B->columns == cols_B, "Matrix B columns initialized");

    matrix *result = matrix_multiplication(A, B);
    test_assert(result, "Result matrix allocated");
    test_assert(result->rows == rows_A, "Result matrix rows initialized");
    test_assert(result->columns == cols_B, "Result matrix columns initialized");

    for (int i = 0; i < rows_A; i++) {
        for (int j = 0; j < cols_B; j++) {
            NN_TYPE expected = 0.0;
            for (int k = 0; k < cols_A; k++) {
                expected += MATRIX(A, i, k) * MATRIX(B, k, j);
            }
            test_assert(MATRIX(result, i, j) == expected,
                        "Result matrix element equals expected value");
        }
    }

    int r = number_delete(A);
    test_assert(r == 0, "Matrix A deleted");

    r = number_delete(B);
    test_assert(r == 0, "Matrix B deleted");

    r = number_delete(result);
    test_assert(r == 0, "Result matrix deleted");

    return 0;
}

float sigmoid(float value) { return 1.f / (1.f + exp(value * -1.f)); }
int   test_matrix_map()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *original = matrix_create_from_list(rows, cols, values);
    test_assert(original, "Original matrix allocated");
    test_assert(original->rows == rows, "Original matrix rows initialized");
    test_assert(original->columns == cols,
                "Original matrix columns initialized");

    matrix *mapped = matrix_map(original, sigmoid);
    test_assert(mapped, "Mapped matrix allocated");
    test_assert(mapped->rows == rows, "Mapped matrix rows initialized");
    test_assert(mapped->columns == cols, "Mapped matrix columns initialized");

    MATRIX_FOREACH(original)
    {
        test_assert(MATRIX(mapped, row, column)
                        == sigmoid(values[row * cols + column]),
                    "Mapped matrix element equals expected value");
    }

    int r = number_delete(mapped);
    test_assert(r == 0, "Matrix deleted");

    return 0;
}

int test_matrix_sum()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *A        = matrix_create_from_list(rows, cols, values);
    test_assert(A, "Matrix A allocated");
    test_assert(A->rows == rows, "Matrix A rows initialized");
    test_assert(A->columns == cols, "Matrix A columns initialized");

    float expected_sum = 21.0;
    float sum          = matrix_sum(A);
    test_assert(sum == expected_sum, "Matrix sum equals expected value");

    int r = number_delete(A);
    test_assert(r == 0, "Matrix A deleted");

    return 0;
}


int test_matrix_trace()
{
    int     rows     = 3;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    matrix *A        = matrix_create_from_list(rows, cols, values);
    test_assert(A, "Matrix A allocated");
    test_assert(A->rows == rows, "Matrix A rows initialized");
    test_assert(A->columns == cols, "Matrix A columns initialized");

    float expected_trace = 15.0;
    float trace          = matrix_trace(A);
    test_assert(trace == expected_trace, "Matrix trace equals expected value");

    int r = number_delete(A);
    test_assert(r == 0, "Matrix A deleted");

    return 0;
}

int test_matrix_frobenius_norm()
{
    int     rows     = 2;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix *A        = matrix_create_from_list(rows, cols, values);
    test_assert(A, "Matrix A allocated");
    test_assert(A->rows == rows, "Matrix A rows initialized");
    test_assert(A->columns == cols, "Matrix A columns initialized");

    float expected_norm = sqrtf(91.0);
    float norm          = matrix_frobenius_norm(A);
    test_assert(norm == expected_norm,
                "Matrix Frobenius norm equals expected value");

    int r = number_delete(A);
    test_assert(r == 0, "Matrix A deleted");

    return 0;
}

int test_matrix_frobenius_norm_by_trace()
{
    int     rows     = 3;
    int     cols     = 3;
    NN_TYPE values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    matrix *A        = matrix_create_from_list(rows, cols, values);
    test_assert(A, "Matrix A allocated");
    test_assert(A->rows == rows, "Matrix A rows initialized");
    test_assert(A->columns == cols, "Matrix A columns initialized");

    float expected_norm = 16.881943;
    float norm          = matrix_frobenius_norm_by_trace(A);
    test_assert(norm == expected_norm,
                "Matrix Frobenius norm by trace equals expected value");

    int r = number_delete(A);
    test_assert(r == 0, "Matrix A deleted");

    return 0;
}

int test_matrix_is_equal()
{
    int     rows       = 2;
    int     cols       = 3;
    NN_TYPE values_A[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    NN_TYPE values_B[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    NN_TYPE values_C[] = {1.0, 2.0, 3.0, 3.0, 5.0, 7.0};
    matrix *A          = matrix_create_from_list(rows, cols, values_A);
    test_assert(A, "Matrix A allocated");
    test_assert(A->rows == rows, "Matrix A rows initialized");
    test_assert(A->columns == cols, "Matrix A columns initialized");

    matrix *B = matrix_create_from_list(rows, cols, values_B);
    test_assert(B, "Matrix B allocated");
    test_assert(B->rows == rows, "Matrix B rows initialized");
    test_assert(B->columns == cols, "Matrix B columns initialized");

    matrix *C = matrix_create_from_list(rows, cols, values_C);
    test_assert(C, "Matrix C allocated");
    test_assert(C->rows == rows, "Matrix C rows initialized");
    test_assert(C->columns == cols, "Matrix C columns initialized");

    test_assert(matrix_is_equal(A, B), "Matrix A is equal to matrix B");
    test_assert(matrix_is_equal(A, C) == 0,
                "Matrix A is not equal to matrix C");

    int r = number_delete(A);
    test_assert(r == 0, "Matrix A deleted");

    r = number_delete(B);
    test_assert(r == 0, "Matrix B deleted");

    r = number_delete(C);
    test_assert(r == 0, "Matrix C deleted");

    return 0;
}

int main()
{
    test_matrix_create();
    test_matrix_seed();
    test_matrix_identity();
    test_matrix_create_from_list();
    test_matrix_from_vector();
    test_matrix_from_vectors();
    test_matrix_diagonal_from_vector();
    test_matrix_clone();
    test_matrix_reshape();
    test_matrix_transpose();
    test_matrix_multiplication();
    test_matrix_map();
    test_matrix_sum();
    test_matrix_trace();
    test_matrix_frobenius_norm();
    test_matrix_frobenius_norm_by_trace();
    test_matrix_is_equal();
    test_vector_transformation_by_matrix();
}