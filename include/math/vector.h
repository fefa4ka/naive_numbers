#pragma once

#include <stdlib.h>

enum nn_type {
    NN_INTEGER,
    NN_FLOAT,
    NN_DOUBLE,
    NN_VECTOR,
    NN_MATRIX,
    NN_TENSOR,
    NN_RATIONAL,
    NN_IRRATIONAL,
    NN_COMPLEX,
    NN_HYPERCOMPLEX,
    NN_QUANTERNION,
    NN_BIG,
    NN_UNDEFINED
};

#ifndef NN_TYPE
    #define NN_TYPE float
#endif

struct nn_number {
    /* used to store any type of numbers */
    union {
        unsigned int integer;
        float        floated;
        double       doubled;
        void        *values;
    };
    enum nn_type type;
};

struct nn_vector {
    struct nn_number number;
    size_t            length;
};

struct nn_matrix {
    struct nn_number number;
    size_t            rows;
    size_t            columns;
};

struct nn_tensor {
    struct nn_number number;
    size_t            rank;
    size_t           *shape;
};

typedef struct nn_number number;
typedef struct nn_vector vector;

#define VECTOR(vector, index)                                                  \
    *((NN_TYPE *)(((number *)vector)->values) + index)
#define VECTOR_FOREACH(vector)                                                 \
    for (size_t index = 0; index < (vector)->length; index++)
#define VECTOR_VEC_FOREACH(vector, power)                                      \
    for (size_t index = 0; index < (vector)->length; index = index + power)


/* Vector verifying */
#define NUMBER_CHECK_LOG(instance, message, ...)                               \
    {                                                                          \
        CHECK_MEMORY_LOG(instance, message, ##__VA_ARGS__);                    \
        CHECK(((number *)instance)->type >= NN_INTEGER                        \
                  && ((number *)instance)->type < NN_UNDEFINED,               \
              "Number with wrong type. " message, ##__VA_ARGS__);              \
    }
#define NUMBER_CHECK(vector) NUMBER_CHECK_LOG(vector, "")
#define VECTOR_CHECK_LOG(vector, message, ...)                                 \
    {                                                                          \
        NUMBER_CHECK_LOG(vector, message, ##__VA_ARGS__);                      \
        CHECK((vector)->length, "Vector length doesn't set. " message,         \
              ##__VA_ARGS__);                                                  \
    }
#define VECTOR_CHECK(vector) VECTOR_CHECK_LOG(vector, "")

/* The v2sf, v4sf, v8sf, etc. are types that are defined using the
 * `__attribute__((vector_size (size)))` attribute, which allows for the
 * creation of custom vector types. These types are used to perform SIMD */
typedef int v2si __attribute__((vector_size(8)));
typedef int v4si __attribute__((vector_size(16)));
typedef int v8si __attribute__((vector_size(32)));
typedef int v16si __attribute__((vector_size(64)));
typedef int v32si __attribute__((vector_size(128)));
typedef int v64si __attribute__((vector_size(256)));
typedef int v128si __attribute__((vector_size(512)));

typedef float v2sf __attribute__((vector_size(8)));
typedef float v4sf __attribute__((vector_size(16)));
typedef float v8sf __attribute__((vector_size(32)));
typedef float v16sf __attribute__((vector_size(64)));
typedef float v32sf __attribute__((vector_size(128)));
typedef float v64sf __attribute__((vector_size(256)));
typedef float v128sf __attribute__((vector_size(512)));
typedef float v256sf __attribute__((vector_size(1024)));

number *integer_create(unsigned int value);
number *float_create(float value);
number *double_create(double value);
int     number_delete(void *number);

vector *vector_create(size_t length);
vector *vector_from_list(size_t length, NN_TYPE values[]);
vector *vector_clone(vector *original);

vector *vector_reshape(vector *instance, size_t length);

vector *vector_addition(vector *v, number *w);
vector *vector_substraction(vector *v, number *w);
vector *vector_multiplication(vector *v, number *w);
vector *vector_division(vector *v, number *w);

NN_TYPE vector_dot_product(vector *v, vector *w);

vector *vector_map(vector *v, NN_TYPE operation(NN_TYPE));
vector *vector_map_value(vector *v, NN_TYPE operation(NN_TYPE, NN_TYPE*), NN_TYPE* value);

int vector_index_of(vector *v, float needle);
NN_TYPE vector_length(vector *v);
NN_TYPE vector_sum(vector *v);
NN_TYPE vector_sum_to(vector *v, size_t to_index);
NN_TYPE vector_sum_between(vector *v, size_t from_index, size_t to_index);

vector *vector_unit(vector *v);
NN_TYPE vector_l_norm(vector *v, int power);
NN_TYPE vector_max_norm(vector *v);
size_t vector_max_index(vector *v);
NN_TYPE vector_angle(vector *v, vector *w);
int vector_is_perpendicular(vector *v, vector *w);
int vector_is_equal(vector *v, vector *w);

void vector_print(vector *instance);

