#include "matrix.h"
#include "number.h"
#include "vector.h"
#include <math.h>


#define MATRIX_INIT(rows_nr, columns_nr)                                       \
    matrix *instance;                                                          \
    CHECK(rows_nr > 0 && columns_nr > 0, "Wrong matrix size");                 \
                                                                               \
    instance = malloc(sizeof(matrix));                                         \
    CHECK_MEMORY(instance);                                                    \
                                                                               \
    instance->number.type      = NN_MATRIX;                                    \
    instance->number.ref_count = 1;                                            \
    instance->rows             = rows_nr;                                      \
    instance->columns          = columns_nr;

#define MATRIX_OPERATION(A, B, expression)                                     \
    MATRIX_FOREACH(A)                                                          \
    {                                                                          \
        MATRIX(A, row, column)                                                 \
            = MATRIX(A, row, column) expression MATRIX(B, row, column);        \
    }

matrix *matrix_create(size_t rows, size_t columns)
{
    MATRIX_INIT(rows, columns);

    instance->number.values = vector_create(rows * columns);
    CHECK_MEMORY(instance->number.values);

    return instance;

error:
    return NULL;
}

matrix *matrix_seed(matrix *instance, NN_TYPE default_value)
{
    MATRIX_CHECK(instance);

    vector_seed(instance->number.values, default_value);

    return instance;

error:
    return NULL;
}

matrix *matrix_identity(size_t size, NN_TYPE default_value)
{
    matrix *instance;

    instance = matrix_create(size, size);
    MATRIX_CHECK(instance);

    while (size--) {
        MATRIX(instance, size, size) = default_value;
    }

    return instance;

error:
    return NULL;
}

matrix *matrix_create_from_list(size_t rows, size_t columns, NN_TYPE values[])
{
    MATRIX_INIT(rows, columns);

    instance->number.values = vector_from_list(rows * columns, values);
    CHECK_MEMORY(instance->number.values);

    return instance;

error:
    return NULL;
}

matrix *matrix_from_vector(vector *v, size_t columns)
{
    VECTOR_CHECK(v);
    CHECK(columns, "Matrix should have at least one column");

    MATRIX_INIT(v->length / columns, columns);

    instance->number.values = vector_clone(v);
    CHECK_MEMORY(instance->number.values);

    return instance;

error:
    return NULL;
}

matrix *matrix_from_vectors(vector **vectors, size_t rows, size_t columns)
{
    matrix *joined;

    CHECK_MEMORY(vectors);
    CHECK(rows > 0 && columns > 0, "Invalid matrix shape");

    joined = matrix_create(rows, columns);
    MATRIX_CHECK(joined);

    MATRIX_FOREACH(joined)
    {
        MATRIX(joined, row, column) = VECTOR(vectors[row], column);
    }

    return joined;

error:
    return NULL;
}

// matrix *matrix_diagonal_from_list(NN_TYPE values[])
matrix *matrix_diagonal_from_vector(vector *v)
{
    matrix *instance;
    size_t  index;

    VECTOR_CHECK(v);

    instance = matrix_create(v->length, v->length);
    MATRIX_CHECK(instance);

    index = v->length;
    while (index--) {
        MATRIX(instance, index, index) = VECTOR(v, index);
    }

    return instance;

error:
    return NULL;
}


matrix *matrix_clone(matrix *original)
{
    MATRIX_INIT(original->rows, original->columns);

    instance->number.values = vector_clone((vector *)original->number.values);
    CHECK_MEMORY(instance->number.values);

    return instance;

error:
    return NULL;
}

matrix *matrix_reshape(matrix *instance, size_t rows, size_t columns)
{
    MATRIX_CHECK(instance);
    CHECK(rows > 0 && columns > 0, "Invalid matrix shape");

    instance->rows    = rows;
    instance->columns = columns;

    vector_reshape(instance->number.values, rows * columns);
    CHECK_MEMORY(instance->number.values);

    return instance;

error:
    return NULL;
}
/* TODO: Think about order inside vector matrix_shrink ? */

vector *matrix_column_vector(matrix *A, size_t column)
{
    vector *column_vector;
    size_t  vector_size;

    CHECK(A->columns > column, "Invalid matrix column");
    MATRIX_CHECK(A);

    vector_size   = A->rows;
    column_vector = vector_create(vector_size);
    VECTOR_CHECK(column_vector);
    for (size_t row = 0; row < vector_size; row++) {
        VECTOR(column_vector, row) = MATRIX(A, row, column);
    }

    return column_vector;

error:
    return NULL;
}

matrix *matrix_sub_matrix(matrix *A, size_t from_row, size_t from_column,
                          size_t rows, size_t columns)
{
    matrix *sub_matrix;

    MATRIX_CHECK(A);
    CHECK(from_row + rows <= A->rows, "Invalid matrix row");
    CHECK(from_column + columns <= A->columns, "Invalid matrix column");

    sub_matrix = matrix_create(rows, columns);
    MATRIX_CHECK(sub_matrix);

    MATRIX_FOREACH(sub_matrix)
    {
        MATRIX(sub_matrix, row, column)
            = MATRIX(A, from_row + row, from_column + column);
    }

    return sub_matrix;

error:
    return NULL;
}

matrix *matrix_minor_matrix(matrix *A, size_t exclude_row,
                            size_t exclude_column)
{
    matrix *minor_matrix;

    MATRIX_CHECK(A);
    CHECK(exclude_row < A->rows && exclude_column < A->columns,
          "Invalid matrix minor");

    minor_matrix = matrix_create(A->rows - 1, A->columns - 1);
    MATRIX_CHECK(minor_matrix);

    MATRIX_FOREACH(A)
    {
        size_t minor_row;
        size_t minor_column;

        if (row == exclude_row || column == exclude_column) {
            continue;
        }

        minor_row = row > exclude_row ? row - 1 : row;

        minor_column = column > exclude_column ? column - 1 : column;

        MATRIX(minor_matrix, minor_row, minor_column) = MATRIX(A, row, column);
    }

    return minor_matrix;

error:
    return NULL;
}

matrix *matrix_transpose(matrix *instance)
{
    matrix *transposed;
    size_t  transposed_rows;
    size_t  instance_rows;
    size_t  rows;

    MATRIX_CHECK(instance);

    instance_rows   = instance->rows;
    transposed_rows = instance->columns;

    transposed = matrix_create(transposed_rows, instance_rows);
    MATRIX_CHECK(transposed);

    rows = transposed_rows;
    while (rows--) {
        size_t columns = transposed->columns;
        while (columns--) {
            if (instance_rows == transposed_rows) {
                if (columns > rows) {
                    MATRIX(instance, rows, columns)
                        += MATRIX(instance, columns, rows);
                    MATRIX(instance, columns, rows)
                        = MATRIX(instance, rows, columns)
                          - MATRIX(instance, columns, rows);
                    MATRIX(instance, rows, columns)
                        -= MATRIX(instance, columns, rows);
                } else {
                    columns = 0;
                }
            } else {
                MATRIX(transposed, rows, columns)
                    = MATRIX(instance, columns, rows);
            }
        }
    }

    if (instance_rows != transposed_rows) {
        number_delete(instance);
        instance = transposed;
    } else {
        number_delete(transposed);
    }

    MATRIX_CHECK(instance);
    return instance;

error:
    return NULL;
}

vector *vector_transformation_by_matrix(matrix *A, vector *x)
{
    vector *transormed_vector;

    MATRIX_CHECK(A);
    VECTOR_CHECK(x);

    transormed_vector = vector_create(A->rows);
    VECTOR_CHECK(transormed_vector);
    for (size_t column = 0; column < A->columns; column++) {
        vector *column_vector;
        number *column_number;

        column_vector = matrix_column_vector(A, column);
        VECTOR_CHECK(column_vector);

        column_number = number_from_vector(x, column);
        NUMBER_CHECK(column_number);

        vector_addition(transormed_vector, (number *)vector_multiplication(
                                               column_vector, column_number));

        number_delete(column_vector);
        number_delete(column_number);
    }

    number_unref((number*)x);

    return transormed_vector;

error:
    return NULL;
}

matrix *matrix_multiplication(matrix *A, matrix *B)
{
    matrix *multiplicated;
    MATRIX_CHECK(A);
    MATRIX_CHECK(B);

    multiplicated = matrix_create(A->rows, B->columns);
    MATRIX_CHECK(multiplicated);
    CHECK(A->rows == B->columns, "Matrix sizes doesn't match");

    MATRIX_FOREACH(multiplicated)
    {
        size_t index = A->columns > 1 ? B->rows : A->columns;

        while (index--) {
            MATRIX(multiplicated, row, column)
                += MATRIX(A, row, index) * MATRIX(B, index, column);
        }
    }

    number_unref((number*)B);

    return multiplicated;

error:
    return NULL;
}

matrix *matrix_map(matrix *A, NN_TYPE operation(NN_TYPE))
{
    MATRIX_CHECK(A);

    vector_map(A->number.values, operation);

    return A;

error:
    return NULL;
}

matrix *matrix_map_value(matrix *A, NN_TYPE operation(NN_TYPE, NN_TYPE *),
                         NN_TYPE *value)
{
    MATRIX_CHECK(A);

    vector_map_value(A->number.values, operation, value);

    return A;

error:
    return NULL;
}

NN_TYPE matrix_sum(matrix *A)
{
    MATRIX_CHECK(A);

    return vector_sum(A->number.values);

error:
    return NAN;
}

NN_TYPE matrix_trace(matrix *A)
{
    NN_TYPE sum;
    size_t  index;

    MATRIX_CHECK(A);

    sum = 0;

    index = A->columns;
    while (index--) {
        sum += MATRIX(A, index, index);
    }

    return sum;

error:
    return NAN;
}

NN_TYPE matrix_frobenius_norm(matrix *A)
{
    NN_TYPE sum;
    matrix *product;
    MATRIX_CHECK(A);

    product = matrix_clone(A);
    MATRIX_CHECK(product);

    MATRIX_OPERATION(product, product, *);
    sum = matrix_sum(product);

    number_delete(product);

    return sqrt(sum);

error:
    return NAN;
}

NN_TYPE matrix_frobenius_norm_by_trace(matrix *instance)
{
    NN_TYPE frobenius;
    matrix *A;
    matrix *AT;
    matrix *A_AT;
    MATRIX_CHECK(instance);

    A = matrix_clone(instance);
    MATRIX_CHECK(A);
    AT = matrix_clone(instance);
    MATRIX_CHECK(AT);
    matrix_transpose(AT);

    A_AT = matrix_multiplication(A, AT);

    frobenius = sqrt(matrix_trace(A_AT));

    number_delete(A);
    number_delete(AT);
    number_delete(A_AT);

    return frobenius;

error:
    return NAN;
}

int matrix_is_equal(matrix *A, matrix *B)
{
    MATRIX_CHECK(A);
    MATRIX_CHECK(B);

    return vector_is_equal(A->number.values, B->number.values);

error:
    return -1;
}

int matrix_lu_decomposition(matrix *A, matrix **L, matrix **U)
{
    matrix *_L;
    matrix *_U;
    size_t  rank;

    MATRIX_CHECK(A);
    CHECK(A->rows > 0, "Matrix is empty");
    CHECK(A->rows == A->columns, "Matrix is not square");

    _L   = matrix_identity(A->rows, 1);
    _U   = matrix_clone(A);
    rank = A->rows;
    MATRIX_CHECK(_L);
    MATRIX_CHECK(_U);

    // Gauss-Jordan elimination
    // https://en.wikipedia.org/wiki/LU_decomposition#pragma omp parallel for
    for (size_t pivot = 0; pivot < rank; pivot++) {
        for (size_t row = pivot + 1; row < rank; row++) {
            NN_TYPE factor = MATRIX(_U, row, pivot) / MATRIX(_U, pivot, pivot);
            MATRIX(_L, row, pivot) = factor;
            for (size_t col = pivot; col < rank; col++) {
                MATRIX(_U, row, col) -= factor * MATRIX(_U, pivot, col);
                MATRIX(_U, row, col)
                    = (fabs(MATRIX(_U, row, col)) < NN_TYPE_EPSILON)
                          ? 0.0
                          : MATRIX(_U, row, col);
            }
        }
    }

    *L = _L;
    *U = _U;

    return rank;

error:
    if (_L)
        number_delete(_L);
    if (_U)
        number_delete(_U);

    return 0;
}

NN_TYPE matrix_determinant(matrix *A)
{
    NN_TYPE determinant;

    MATRIX_CHECK(A);
    CHECK(A->rows == A->columns, "Matrix is not square");

    if (A->rows == 1 && A->columns == 1) {
        determinant = MATRIX(A, 0, 0);
    } else if (A->rows == 2 && A->columns == 2) {
        determinant = MATRIX(A, 0, 0) * MATRIX(A, 1, 1)
                      - MATRIX(A, 0, 1) * MATRIX(A, 1, 0);
    } else if (A->rows == 3 && A->columns == 3) {
        // Using Laplace expansion
        // https://en.wikipedia.org/wiki/Laplace_expansion
        determinant = 0;
        for (size_t column = 0; column < A->columns; column++) {
            matrix *sub_matrix;
            NN_TYPE cofactor;
            sub_matrix = matrix_minor_matrix(A, 0, column);
            CHECK_MEMORY(sub_matrix);
            cofactor = matrix_determinant(sub_matrix)
                       * ((column % 2 ? -1 : 1) * MATRIX(A, 0, column));
            determinant += cofactor;
            number_delete(sub_matrix);
        }
    } else {
        // Using LU decomposition
        matrix *L;
        matrix *U;
        int     rank = matrix_lu_decomposition(A, &L, &U);
        CHECK(rank == A->rows, "Matrix LU decomposition rank equals %d", rank);
        determinant = 1;
        matrix_print(U);
        for (size_t row = 0; row < A->rows; row++) {
            determinant *= MATRIX(U, row, row);
        }
        number_delete(L);
        number_delete(U);
    }

    return determinant;

error:
    return NAN;
}

void matrix_print(matrix *instance)
{
    MATRIX_CHECK(instance);

    size_t previous_row = 0;

    printf("\tMatrix: %ldx%ld\n\t\t[[\t",
           (int)((vector *)(instance->number.values))->length
               / instance->columns,
           instance->columns);

    MATRIX_FOREACH(instance)
    {
        int is_head_or_tail = row < 5 || row > instance->rows - 5;
        int is_middle       = row == 6;
        int is_shown        = is_head_or_tail || is_middle;

        if (row != previous_row && is_shown) {
            if (row > 0) {
                printf("],\n\t\t");
            }
            printf("[\t");
        }
        if (is_head_or_tail) {
            printf("%f\t\t", MATRIX(instance, row, column));
        }

        if (is_middle) {
            printf("...\t\t\t");
        }

        previous_row = row;
    }

    printf("]]");

    printf("\n\t\tFrobenius Norm: %f\n", matrix_frobenius_norm(instance));

error:
    return;
}
