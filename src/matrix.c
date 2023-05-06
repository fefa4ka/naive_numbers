#include "matrix.h"
#include "vector.h"
#include <math.h>


#define MATRIX_INIT(rows_nr, columns_nr)                                       \
    matrix *instance;                                                          \
    CHECK(rows_nr > 0 && columns_nr > 0, "Wrong matrix size");                 \
                                                                               \
    instance = malloc(sizeof(matrix));                                         \
    CHECK_MEMORY(instance);                                                    \
                                                                               \
    instance->number.type = NN_MATRIX;                                         \
    instance->rows        = rows_nr;                                           \
    instance->columns     = columns_nr;

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

matrix *matrix_identity(size_t size)
{
    matrix *instance;

    instance = matrix_create(size, size);
    MATRIX_CHECK(instance);

    while (size--) {
        MATRIX(instance, size, size) = 1;
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

    return multiplicated;

error:
    return NULL;
}

matrix *matrix_map(matrix *A, float operation(float))
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

float matrix_sum(matrix *A)
{
    MATRIX_CHECK(A);

    return vector_sum(A->number.values);

error:
    return 0;
}

float matrix_trace(matrix *A)
{
    float  sum;
    size_t index;

    MATRIX_CHECK(A);

    sum = 0;

    index = A->columns;
    while (index--) {
        sum += MATRIX(A, index, index);
    }

    return sum;

error:
    return 0;
}

float matrix_frobenius_norm(matrix *A)
{
    float   sum;
    matrix *product;
    MATRIX_CHECK(A);

    product = matrix_clone(A);
    MATRIX_CHECK(product);

    MATRIX_OPERATION(product, product, *);
    sum = matrix_sum(product);

    number_delete(product);

    return sqrt(sum);

error:
    return 0;
}

float matrix_frobenius_norm_by_trace(matrix *instance)
{
    float   frobenius;
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
    return 0;
}

int matrix_is_equal(matrix *A, matrix *B)
{
    MATRIX_CHECK(A);
    MATRIX_CHECK(B);

    return vector_is_equal(A->number.values, B->number.values);

error:
    return -1;
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
