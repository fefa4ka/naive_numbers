#include "number.h"

#include "util/error.h"
#include <stdio.h>

int vector_delete(vector *instance);
int text_delete(text *instance);

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

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    instance->type    = NN_TYPE_ENUM;
    instance->floated = value;

    return instance;

error:
    return NULL;
}

number *integer_create(unsigned int value)
{
    number *instance;

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    instance->type    = NN_INTEGER;
    instance->integer = value;

    return instance;

error:
    return NULL;
}

number *float_create(float value)
{
    number *instance;

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    instance->type    = NN_FLOAT;
    instance->floated = value;

    return instance;

error:
    return NULL;
}

number *double_create(double value)
{
    number *instance;

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    instance->type    = NN_DOUBLE;
    instance->doubled = value;

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
    if (NN_FLOAT >= instance->type) {
        free(instance);
    } else if (NN_VECTOR == instance->type) {
        r = vector_delete((vector *)instance);
        CHECK(r == 0, "vector_delete() failed");
    } else if (NN_MATRIX == instance->type) {
        r = vector_delete((vector *)instance->values);
        CHECK(r == 0, "vector_delete() failed");
        free(instance);
    } else if (NN_TEXT == instance->type) {
        r = text_delete((text *)instance);
        CHECK(r == 0, "vector_delete() failed");
    }
    // TODO: tensor, ...

    return 0;

error:
    return 1;
}

int vector_delete(vector *vector)
{
    int r;
    CHECK_MEMORY(vector);

    if (NN_VECTOR == vector->number.type) {
        CHECK_MEMORY(vector->number.values);
        free(vector->number.values);
        free(vector);
    }

    return 0;

error:
    return 1;
}

int text_delete(text *instance)
{
    int r;
    CHECK_MEMORY(instance);

    if (NN_TEXT == instance->number.type) {
        CHECK_MEMORY(instance->number.values);
        free(instance->number.values);
        free(instance);
    }

    return 0;

error:
    return 1;
}

