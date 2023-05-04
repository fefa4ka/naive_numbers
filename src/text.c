#include "text.h"

/**
 * Creates a new text instance with the given length.
 * 
 * @param length The length of the text instance to create.
 * @return A pointer to the newly created text instance, or NULL if an error occurred.
 */
text *text_create(size_t length)
{
    text *instance;
    NN_CHAR_TYPE *values;

    instance = malloc(sizeof(text));
    CHECK_MEMORY(instance);

    values = calloc(length, sizeof(NN_CHAR_TYPE));
    CHECK_MEMORY_LOG(values, "Size: %lu", length);

    instance->number.type   = NN_TEXT;
    instance->length        = length;
    instance->number.values = values;

    return instance;

error:
    if (instance)
        free(instance);

    return NULL;
}
