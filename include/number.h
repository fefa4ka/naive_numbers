#pragma once

#include <stdlib.h>
#include "util/error.h"

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
    NN_TEXT,
    NN_UNDEFINED
};

#ifndef NN_TYPE
    #define NN_TYPE float
    #define NN_TYPE_ENUM NN_FLOAT
#endif

#ifndef NN_CHAR_TYPE
    #define NN_CHAR_TYPE char 
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
    size_t           length;
};

struct nn_text {
    struct nn_number number;
    size_t length;
};

struct nn_matrix {
    struct nn_number number;
    size_t           rows;
    size_t           columns;
};

struct nn_tensor {
    struct nn_number number;
    size_t           rank;
    size_t          *shape;
};


typedef struct nn_number number;
typedef struct nn_vector vector;
typedef struct nn_text text;
typedef struct nn_matrix matrix;
typedef struct nn_tensor tensor;


struct nn_probability 
{
    char **fields;
    matrix *samples;

    vector **events;
    vector **occurs;
    vector **P;

    float *variance;
    matrix *covariance;
    matrix *correlation;
};
typedef struct nn_probability probability;



number *number_create(NN_TYPE value);
number *integer_create(unsigned int value);
number *float_create(float value);
number *double_create(double value);
int     number_delete(void *number);


static inline NN_TYPE nn_random_range(NN_TYPE min, NN_TYPE max)
{
    NN_TYPE range = (max - min);
    NN_TYPE div = RAND_MAX / range;
    NN_TYPE random = min + (rand() / div);

    return random > 1e-5 ? random : 1e-5;
}

/* Vector verifying */
#define NUMBER_CHECK_LOG(instance, message, ...)                               \
    {                                                                          \
        CHECK_MEMORY_LOG(instance, message, ##__VA_ARGS__);                    \
        CHECK(((number *)instance)->type >= NN_INTEGER                         \
                  && ((number *)instance)->type < NN_UNDEFINED,                \
              "Number with wrong type. " message, ##__VA_ARGS__);              \
    }
#define NUMBER_CHECK(vector) NUMBER_CHECK_LOG(vector, "")
