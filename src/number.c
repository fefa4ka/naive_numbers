#include "number.h"

#include "util/error.h"
#include "vector.h"
#include <stdio.h>

int object_delete(number *instance);
int matrix_delete(number *instance);

/**
 * Creates a new number instance with the given value of defaule NN_TYPE.
 *
 * @param value The value to assign to the new number instance.
 * @return A pointer to the newly created number instance.
 *
 * @throws NULL if there is not enough memory to allocate the new instance.
 *
 * This function creates a new number instance with the given value. The new instance is allocated on the heap using malloc().
 * If there is not enough memory to allocate the new instance, NULL is thrown.
 *
 */
number *number_create(NN_TYPE value)
{
    number *instance;

    instance = malloc(sizeof(number));
    CHECK_MEMORY(instance);

    instance->type    = NN_TYPE_ENUM;
    instance->floated = value;
    instance->ref_count = 1;

    return instance;

error:
    return NULL;
}

number *integer_create(int value)
{
    number *instance;

    instance = malloc(sizeof(number));
    CHECK_MEMORY(instance);

    instance->type    = NN_INTEGER;
    instance->integer = value;
    instance->ref_count = 1;

    return instance;

error:
    return NULL;
}

number *float_create(float value)
{
    number *instance;

    instance = malloc(sizeof(number));
    CHECK_MEMORY(instance);

    instance->type    = NN_FLOAT;
    instance->floated = value;
    instance->ref_count = 1;

    return instance;

error:
    return NULL;
}

number *double_create(double value)
{
    number *instance;

    instance = malloc(sizeof(number));
    CHECK_MEMORY(instance);

    instance->type    = NN_DOUBLE;
    instance->doubled = value;
    instance->ref_count = 1;

    return instance;

error:
    return NULL;
}

int number_delete(void *number_ptr)
{
    int     r;
    number *instance;
    CHECK_MEMORY(number_ptr);

    instance = (number *)number_ptr;
    if (NN_DOUBLE >= instance->type) {
        free(instance);
    } else if (NN_VECTOR == instance->type) {
        r = object_delete(instance);
        CHECK(r == 0, "vector_delete() failed");
    } else if (NN_MATRIX == instance->type) {
        r = matrix_delete(instance);
        CHECK(r == 0, "vector_delete() failed");
    }
    // TODO: tensor, ...

    return 0;

error:
    return 1;
}

int object_delete(number *instance)
{
    int r;
    
    CHECK_MEMORY(instance);
    CHECK_MEMORY(instance->values);

    free(instance->values);
    free(instance);

    return 0;

error:
    return 1;
}

int matrix_delete(number *instance)
{
    int r;
    
    CHECK_MEMORY(instance);
    CHECK_MEMORY(instance->values);

    object_delete(instance->values);
    free(instance);

    return 0;

error:
    return 1;
}

number *number_ref(number *n) {
     if (n) {
         atomic_fetch_add(&n->ref_count, 1);
     }
     return n;
 }

void number_unref(number *n) {
     if (n && atomic_load(&n->ref_count) > 0) {
         if (atomic_fetch_sub(&n->ref_count, 1) == 1) {
             // This was the last reference
             number_delete(n);
         }
     }
 }

