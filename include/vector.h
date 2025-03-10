#pragma once

#include "number.h"
#include <simde/x86/sse2.h>

#define VECTOR(vector, index) *((NN_TYPE *)(((number *)vector)->values) + index)
#define VECTOR_FOREACH(vector)                                                 \
    for (size_t index = 0; index < (vector)->length; index++)

#define number_from_vector(v, index) number_create(VECTOR(v, index))

vector *vector_create(size_t length);
vector *vector_seed(vector *instance, NN_TYPE default_value);
vector *vector_from_list(size_t length, NN_TYPE values[]);
vector *vector_unique(const vector *instance);
vector *vector_clone(const vector *original);
vector *vector_reshape(vector *instance, size_t length);
vector *vector_shuffle(const vector *instance);

vector *vector_addition(vector *v, const number *w);
vector *vector_subtraction(vector *v, const number *w);
vector *vector_multiplication(vector *v, const number *w);
vector *vector_division(vector *v, const number *w);

NN_TYPE vector_dot_product(const vector *v, const vector *w);
NN_TYPE vector_angle(const vector *v, const vector *w);
int     vector_is_perpendicular(const vector *v, const vector *w);
int     vector_is_equal(const vector *v, const vector *w);

vector *vector_map(vector *v, NN_TYPE operation(NN_TYPE));
vector *vector_map_value(vector *v, NN_TYPE operation(NN_TYPE, NN_TYPE *),
                         NN_TYPE *value);

int     vector_index_of(const vector *v, NN_TYPE needle);
NN_TYPE vector_length(const vector *v);
vector *vector_unit(const vector *v);

NN_TYPE vector_sum(const vector *v);
NN_TYPE vector_sum_to(const vector *v, size_t to_index);
NN_TYPE vector_sum_between(const vector *v, size_t from_index, size_t to_index);

NN_TYPE vector_l_norm(const vector *v, int power);
NN_TYPE vector_max_norm(const vector *v);
size_t  vector_max_index(const vector *v);
size_t  vector_non_zero_length(const vector *v);

void vector_print(const vector *instance);


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

typedef NN_TYPE v2sf __attribute__((vector_size(8)));
typedef NN_TYPE v4sf __attribute__((vector_size(16)));
typedef NN_TYPE v8sf __attribute__((vector_size(32)));
typedef NN_TYPE v16sf __attribute__((vector_size(64)));
typedef NN_TYPE v32sf __attribute__((vector_size(128)));
typedef NN_TYPE v64sf __attribute__((vector_size(256)));
typedef NN_TYPE v128sf __attribute__((vector_size(512)));
typedef NN_TYPE v256sf __attribute__((vector_size(1024)));

#define VECTOR_VEC_FOREACH(vector, power)                                      \
    for (size_t index = 0; index < (vector)->length; index = index + power)


/* Vector verifying */
#define VECTOR_CHECK_LOG(vector, message, ...)                                 \
    {                                                                          \
        NUMBER_CHECK_LOG(vector, message, ##__VA_ARGS__);                      \
        CHECK((vector)->length, "Vector length doesn't set. " message,         \
              ##__VA_ARGS__);                                                  \
    }
#define VECTOR_CHECK(vector) VECTOR_CHECK_LOG(vector, "")
