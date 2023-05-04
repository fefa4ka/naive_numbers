#include "utils.h"


NN_TYPE *nn_uniq_numbers(NN_TYPE *values, size_t size, size_t *new_size_ptr) {
    NN_TYPE *uniq;
    
    uniq = malloc(size * sizeof(NN_TYPE));
    CHECK_MEMORY(uniq);
    
    size_t front, back;
    size_t new_size;
    
    new_size = 1;
    uniq[0] = values[0];
    
    for (front = 1; front < size; front++)
    {
        for (back = 0; back < new_size; back++) {
            if(values[front] == uniq[back]) {
                break;
            }
        }
        
        if(back == new_size) {
            uniq[new_size++] = values[front];
        }
    }
    
    uniq = realloc(uniq, new_size * sizeof(NN_TYPE));
    CHECK_MEMORY(uniq);
    *new_size_ptr = new_size;
    
    return uniq;

error:
    if(uniq) free(uniq);
    return NULL;
}