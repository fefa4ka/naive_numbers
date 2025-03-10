#pragma once

#include "vector.h"
#include <stdarg.h>

inline NN_TYPE TENSOR(tensor *T, ...)
{
    size_t index;
    va_list args;
    va_start(args, T);
    index = 0;
    for (size_t i = 0; i < T->rank; i++) {
        size_t dim_size, dim_index;
        dim_size = T->shape[i];
        dim_index = va_arg(args, size_t);
        if (dim_index >= dim_size) {
            fprintf(stderr, "Index out of bounds\n");
            exit(EXIT_FAILURE);
        }
        index = index * dim_size + dim_index;
    }
    va_end(args);
    return VECTOR(T->number.values, index);
}

#define number_from_tensor(T, ...) number_create(TENSOR(T, __VA_ARGS__));