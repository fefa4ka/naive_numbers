#include "utils.h"


NN_TYPE *nn_unique_numbers(NN_TYPE *values, size_t size, size_t *new_size_ptr)
{
    NN_TYPE *unique;

    unique = malloc(size * sizeof(NN_TYPE));
    CHECK_MEMORY(unique);

    size_t front, back;
    size_t new_size;

    new_size  = 1;
    unique[0] = values[0];

    for (front = 1; front < size; front++) {
        for (back = 0; back < new_size; back++) {
            if (values[front] == unique[back]) {
                break;
            }
        }

        if (back == new_size) {
            unique[new_size++] = values[front];
        }
    }

    unique = realloc(unique, new_size * sizeof(NN_TYPE));
    CHECK_MEMORY(unique);
    *new_size_ptr = new_size;

    return unique;

error:
    if (unique)
        free(unique);
    return NULL;
}
