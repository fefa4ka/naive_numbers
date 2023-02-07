#include "vector.h"

#include "util/error.h"
// #include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define VECTOR_COMMON_SIZE      128
#define COPY_ALIGMENT           32
#define COPY_ALIGMENT_THRESHOLD 32 * 4
#define PRAGMA(x)               _Pragma(#x)
#define VECTOR_OPERATION(result, v, w, expression)                             \
    VECTOR_FOREACH(result)                                                     \
    {                                                                          \
        VECTOR(result, index) = VECTOR(v, index) expression VECTOR(w, index);  \
    }

#define VECTOR_SCALAR_OPERATION(result, v, value, expression)                  \
    VECTOR_FOREACH(result)                                                     \
    {                                                                          \
        VECTOR(result, index) = VECTOR(v, index) expression value;             \
    }

vector *vector_create(size_t length)
{
    vector  *instance;
    NN_TYPE *values;

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    values = calloc(length, sizeof(NN_TYPE));
    CHECK_MEMORY_LOG(values, "Size: %lu", length);

    instance->number.type   = NN_VECTOR;
    instance->length        = length;
    instance->number.values = values;

    return instance;

error:
    if (instance)
        free(instance);

    return NULL;
}

vector *vector_from_list(size_t length, NN_TYPE values[])
{
    void    *r;
    vector  *instance;
    size_t   vector_length;
    NN_TYPE *vector_values;

    CHECK(length, "Vector length should be greater than zero (length=%ld)",
          length);

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    vector_length = length * sizeof(NN_TYPE);
    vector_values = malloc(vector_length);
    CHECK_MEMORY(vector_values);

    r = memcpy(vector_values, values, vector_length);
    CHECK(r == vector_values, "memcpy() %ld bytes failed", length);

    instance->number.type   = NN_VECTOR;
    instance->length        = length;
    instance->number.values = vector_values;

    return instance;

error:
    if (instance)
        free(instance);
    if (vector_values)
        free(vector_values);

    return NULL;
}

vector *vector_clone(vector *original)
{
    VECTOR_CHECK(original);

    return vector_from_list(original->length, original->number.values);

error:
    return NULL;
}

vector *vector_reshape(vector *instance, size_t length)
{
    void    *r;
    NN_TYPE *reshaped;

    VECTOR_CHECK(instance);

    reshaped = realloc(instance->number.values, length * sizeof(NN_TYPE));
    CHECK_MEMORY(reshaped);
    instance->number.values = reshaped;

    if (length > instance->length) {
        memset((NN_TYPE *)instance->number.values + instance->length, 0,
               (length - instance->length) * sizeof(NN_TYPE));
    }

    instance->length = length;

    return instance;

error:
    return NULL;
}

#define VECTOR_TYPE_OPERATION(v_block, w_block, size, operation)               \
    case size: {                                                               \
        v##size##sf *block = (v##size##sf *)v_block;                           \
                                                                               \
        if (NN_DOUBLE >= w->type) {                                            \
            *block = *block operation w->floated;                              \
        } else if (NN_VECTOR == w->type) {                                     \
            *block = *block operation * (v##size##sf *)w_block;                \
        }                                                                      \
        break;                                                                 \
    }

#define VECTOR_METHOD_OPERATION(name, operation)                               \
    vector *vector_##name(vector *v, number *w)                                \
    {                                                                          \
        int   power;                                                           \
        void *values_added_ptr;                                                \
                                                                               \
        VECTOR_CHECK(v);                                                       \
        NUMBER_CHECK(w);                                                       \
                                                                               \
        /* This code uses the __builtin_clz function, which counts the         \
         * number of leading zero bits in an integer, and bit shifting         \
         * to calculate the nearest power of 2 that is greater than or equal   \
         * to the length of the vector. */                                     \
        power = v->length >= 128                                               \
                    ? 128                                                      \
                    : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));       \
                                                                               \
        PRAGMA(omp for schedule(auto))                                         \
        for (size_t index = 0; index < (v)->length; index = index + power) {   \
            void *block_ptr = (NN_TYPE *)(v->number.values) + index;           \
                                                                               \
            if (NN_VECTOR == w->type) {                                        \
                values_added_ptr = (NN_TYPE *)(w->values) + index;             \
                __builtin_prefetch(values_added_ptr + power, 0, 1);            \
            }                                                                  \
            __builtin_prefetch(block_ptr + power, 1, 1);                       \
                                                                               \
            switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {             \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 128,        \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 64,         \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 32,         \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 16,         \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 8,          \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 4,          \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 2,          \
                                      operation);                              \
            }                                                                  \
        }                                                                      \
                                                                               \
        return v;                                                              \
                                                                               \
    error:                                                                     \
        return NULL;                                                           \
    }

VECTOR_METHOD_OPERATION(addition, +);
VECTOR_METHOD_OPERATION(substraction, -);
VECTOR_METHOD_OPERATION(multiplication, *);
VECTOR_METHOD_OPERATION(division, /);

vector *vector_addition_func(vector *v, number *w)
{
    int   power;
    void *values_added_ptr;

    VECTOR_CHECK(v);
    NUMBER_CHECK(w);

    /* This code uses the __builtin_clz function, which counts the
     * number of leading zero bits in an integer, and bit shifting
     * to calculate the nearest power of 2 that is greater than or equal to
     * the length of the vector. */
    power = v->length >= 128
                ? 128
                : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));

#pragma omp for schedule(auto)
    for (size_t index = 0; index < (v)->length; index = index + power) {
        void *block_ptr = (float *)(v->number.values) + index;

        if (NN_VECTOR == w->type) {
            values_added_ptr = (float *)(w->values) + index;
            __builtin_prefetch(values_added_ptr + power, 0, 1);
        }
        __builtin_prefetch(block_ptr + power, 1, 1);

        switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 128, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 64, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 32, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 16, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 8, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 4, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 2, +);
        }
    }

    return v;

error:
    return NULL;
}

float vector_dot_product(vector *v, vector *w)
{
    VECTOR_CHECK(v);
    VECTOR_CHECK(w);

    float product = 0.0f;

    // #pragma omp parallel for simd reduction (+:product)
    VECTOR_FOREACH(v) { product += VECTOR(v, index) * VECTOR(w, index); }

    return product;

error:
    return 0;
}

#define VECTOR_MAP_OPERATION(v_block, size, operation)                         \
    case size: {                                                               \
        v##size##sf *block = (v##size##sf *)v_block;                           \
        for (size_t index = 0; index < size; index++)                          \
            (*block)[index] = operation((*block)[index]);                      \
        break;                                                                 \
    }
vector *vector_map(vector *v, NN_TYPE operation(NN_TYPE))
{
    int   power;
    void *values_added_ptr;

    VECTOR_CHECK(v);

    power = v->length >= 128
                ? 128
                : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));

#pragma omp for schedule(static)
    for (size_t index = 0; index < (v)->length; index = index + power) {
        void *block_ptr = (float *)(v->number.values) + index;

        __builtin_prefetch(block_ptr + power, 1, 1);

        switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {
            VECTOR_MAP_OPERATION(block_ptr, 128, operation);
            VECTOR_MAP_OPERATION(block_ptr, 64, operation);
            VECTOR_MAP_OPERATION(block_ptr, 32, operation);
            VECTOR_MAP_OPERATION(block_ptr, 16, operation);
            VECTOR_MAP_OPERATION(block_ptr, 8, operation);
            VECTOR_MAP_OPERATION(block_ptr, 4, operation);
            VECTOR_MAP_OPERATION(block_ptr, 2, operation);
        }
    }

    return v;

error:
    return NULL;
}

#define VECTOR_MAP_VALUE_OPERATION(v_block, size, operation, value)            \
    case size: {                                                               \
        v##size##sf *block = (v##size##sf *)v_block;                           \
        for (size_t index = 0; index < size; index++)                          \
            (*block)[index] = operation((*block)[index], value);               \
        break;                                                                 \
    }
vector *vector_map_value(vector *v, NN_TYPE operation(NN_TYPE, NN_TYPE *),
                         NN_TYPE *value)
{
    int   power;
    void *values_added_ptr;

    VECTOR_CHECK(v);

    power = v->length >= 128
                ? 128
                : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));

#pragma omp for schedule(static)
    for (size_t index = 0; index < (v)->length; index = index + power) {
        void *block_ptr = (float *)(v->number.values) + index;

        __builtin_prefetch(block_ptr + power, 1, 1);

        switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 128, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 64, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 32, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 16, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 8, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 4, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 2, operation, value);
        }
    }

    return v;

error:
    return NULL;
}

int vector_index_of(vector *v, float needle)
{
    VECTOR_CHECK(v);

    VECTOR_FOREACH(v)
    {
        if (VECTOR(v, index) == needle) {
            return (int)index;
        }
    }

    return -1;

error:
    return -1;
}

NN_TYPE vector_length(vector *v)
{
    VECTOR_CHECK(v);

    return sqrt(vector_dot_product(v, v));

error:
    return 0;
}

NN_TYPE vector_sum(vector *v)
{
    VECTOR_CHECK(v);

    NN_TYPE sum = 0;

    // #pragma omp parallel for simd reduction (+:sum)
    VECTOR_FOREACH(v) { sum += VECTOR(v, index); }

    return sum;

error:
    return 0;
}

NN_TYPE vector_sum_to(vector *v, size_t to_index)
{
    VECTOR_CHECK(v);

    NN_TYPE sum = 0;

    VECTOR_FOREACH(v)
    {
        sum += VECTOR(v, index);
        if (index == to_index) {
            return sum;
        }
    }

    return sum;

error:
    return 0;
}

NN_TYPE vector_sum_between(vector *v, size_t from_index, size_t to_index)
{
    VECTOR_CHECK(v);

    NN_TYPE sum = 0;

    for (size_t index = from_index; index < to_index; index++) {
        sum += VECTOR(v, index);
    }

    return sum;

error:
    return 0;
}

vector *vector_unit(vector *v)
{
    VECTOR_CHECK(v);

    /* FIXME: float_create -> NN_TYPE_create */
    number *length = float_create(vector_length(v));

    return vector_division(v, length);

error:
    return NULL;
}

NN_TYPE vector_l_norm(vector *v, int power)
{
    VECTOR_CHECK(v);
    CHECK(power, "P = 0 for L_norm");

    float l_norm = 0;

    size_t index = v->length;
    while (index--) {
        l_norm += pow(fabs(VECTOR(v, index)), power);
    }

    l_norm = pow(l_norm, (float)1 / power);

    return l_norm;

error:
    return 0;
}

NN_TYPE vector_max_norm(vector *v)
{
    VECTOR_CHECK(v);

    float max = 0;

    size_t index = v->length;
    while (index--) {
        float value = fabs(VECTOR(v, index));
        if (value > max) {
            max = value;
        }
    }

    return max;

error:
    return 0;
}

size_t vector_max_index(vector *v)
{
    VECTOR_CHECK(v);

    float  max       = 0;
    size_t max_index = 0;

    size_t index = v->length;
    while (index--) {
        float value = fabs(VECTOR(v, index));
        if (value > max) {
            max       = value;
            max_index = index;
        }
    }

    return max_index;

error:
    return 0;
}

NN_TYPE vector_angle(vector *v, vector *w)
{
    VECTOR_CHECK(v);
    VECTOR_CHECK(w);

    float cosine
        = vector_dot_product(v, w) / (vector_length(v) * vector_length(w));

    float angle_in_degrees = acos(cosine) * 180 / M_PI;

    return angle_in_degrees;

error:
    return 0;
}

void vector_print(vector *instance)
{
    VECTOR_CHECK(instance);

    printf("\tVector size = %d\n\t\t[\t", (int)instance->length);

    VECTOR_FOREACH(instance)
    {
        if (index < 5 || index > instance->length - 5) {
            printf("%.4f,\n\t\t\t", VECTOR(instance, index));
        }

        if (index == 6) {
            printf("...,\n\t\t\t");
        }
    }

    printf("\t\t\t]\n");

error:
    return;
}
