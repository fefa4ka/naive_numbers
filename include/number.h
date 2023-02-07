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
    size_t           length;
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

typedef struct nn_vector vector;
typedef struct nn_number number;


number *integer_create(unsigned int value);
number *float_create(float value);
number *double_create(double value);
int     number_delete(void *number);


/* Vector verifying */
#define NUMBER_CHECK_LOG(instance, message, ...)                               \
    {                                                                          \
        CHECK_MEMORY_LOG(instance, message, ##__VA_ARGS__);                    \
        CHECK(((number *)instance)->type >= NN_INTEGER                         \
                  && ((number *)instance)->type < NN_UNDEFINED,                \
              "Number with wrong type. " message, ##__VA_ARGS__);              \
    }
#define NUMBER_CHECK(vector) NUMBER_CHECK_LOG(vector, "")
