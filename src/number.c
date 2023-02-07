#include "number.h"

#include "util/error.h"
#include <stdio.h>

int vector_delete(vector *vector);

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
