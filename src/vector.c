#include "vector.h"

#include "number.h"
#include "utils.h"
#include "util/error.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define VECTOR_COMMON_SIZE      128
#define COPY_ALIGMENT           32
#define COPY_ALIGMENT_THRESHOLD 32 * 4
#define PRAGMA(x)               _Pragma(#x)
#define VECTOR_OPERATION(result, v, w, expression)                             \
    VECTOR_FOREACH(result)                                                     \
    {                                                                          \
        VECTOR(result, index) = VECTOR(v, index) expression VECTOR(w, index);  \
    }

#define VECTOR_SCALAR_OPERATION(result, v, value, expression)                  \
    VECTOR_FOREACH(result)                                                     \
    {                                                                          \
        VECTOR(result, index) = VECTOR(v, index) expression value;             \
    }


/**
 * Creates a new vector instance with the specified length.
 *
 * @param length The length of the vector to create.
 * @return A pointer to the newly created vector instance, or NULL if an error occurred.
 *
 * @throws MemoryError if there is not enough memory to allocate the vector or its values.
 *
 * @note The caller is responsible for freeing the memory allocated by this function.
 */
vector *vector_create(size_t length)
{
    vector  *instance;
    NN_TYPE *values;

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    values = calloc(length, sizeof(NN_TYPE));
    CHECK_MEMORY_LOG(values, "Size: %lu", length);

    instance->number.type   = NN_VECTOR;
    instance->length        = length;
    instance->number.values = values;

    return instance;

error:
    if (instance)
        free(instance);

    return NULL;
}

/**
 * Initializes a vector with either a default value or random values within a range.
 *
 * @param instance A pointer to the vector to be initialized.
 * @param default_value The default value to initialize the vector with. If 0, random values will be used instead.
 * @return A pointer to the initialized vector. If an error occurs, NULL is returned.
 *
 * @throws VECTOR_ERROR_NULL_INSTANCE if instance is NULL.
 *
 * @note This function can be parallelized using OpenMP by uncommenting the #pragma directive.
 */
vector *vector_seed(vector *instance, NN_TYPE default_value)
{
    // Check for NULL instance
    VECTOR_CHECK(instance);

    // Seed the vector with either default values or random values
    VECTOR_FOREACH(instance)
    {
        if (default_value) {
            VECTOR(instance, index) = default_value;
        } else {
            VECTOR(instance, index) = nn_random_range(-1, 1);
        }
    }

    // Return the initialized vector
    return instance;

    // Handle errors by returning NULL
error:
    return NULL;
}

/**
 * Creates a new vector instance from an array of NN_TYPE values.
 *
 * @param length The length of the array of values.
 * @param values The array of NN_TYPE values to be used to create the vector.
 *
 * @return A pointer to the newly created vector instance.
 * @retval NULL if an error occurred during creation.
 *
 * @note The caller is responsible for freeing the memory allocated for the vector instance.
 * @note The caller is responsible for freeing the memory allocated for the vector values.
 *
 * @warning The length parameter must be greater than zero.
 * @warning The values parameter must not be NULL.
 * @warning The function will return NULL if an error occurs during creation.
 */
vector *vector_from_list(size_t length, NN_TYPE values[])
{
    void    *r;
    vector  *instance;
    size_t   vector_length;
    NN_TYPE *vector_values;

    CHECK(length, "Vector length should be greater than zero (length=%ld)",
          length);

    instance = malloc(sizeof(vector));
    CHECK_MEMORY(instance);

    vector_length = length * sizeof(NN_TYPE);
    vector_values = malloc(vector_length);
    CHECK_MEMORY(vector_values);

    r = memcpy(vector_values, values, vector_length);
    CHECK(r == vector_values, "memcpy() %ld bytes failed", length);

    instance->number.type   = NN_VECTOR;
    instance->length        = length;
    instance->number.values = vector_values;

    return instance;

error:
    if (instance)
        free(instance);
    if (vector_values)
        free(vector_values);

    return NULL;
}

vector *vector_clone(vector *original)
{
    VECTOR_CHECK(original);

    return vector_from_list(original->length, original->number.values);

error:
    return NULL;
}

/**
 * Reshapes a vector instance to a new length.
 *
 * @param instance A pointer to the vector instance to be reshaped.
 * @param length The new length of the vector.
 *
 * @return A pointer to the reshaped vector instance.
 * @retval NULL If an error occurred during the reshaping process.
 *
 * @throws VECTOR_ERROR_NULL_INSTANCE If the instance pointer is NULL.
 * @throws VECTOR_ERROR_MEMORY If there was an error allocating memory for the reshaped vector.
 *
 * @note If the new length is greater than the current length, the additional elements will be initialized to 0.
 */
vector *vector_reshape(vector *instance, size_t length)
{
    void    *r;
    NN_TYPE *reshaped;

    VECTOR_CHECK(instance);

    // Reallocate memory for the reshaped vector
    reshaped = realloc(instance->number.values, length * sizeof(NN_TYPE));
    CHECK_MEMORY(reshaped);
    instance->number.values = reshaped;

    // Initialize additional elements to 0 if new length is greater than current length
    if (length > instance->length) {
        memset((NN_TYPE *)instance->number.values + instance->length, 0,
               (length - instance->length) * sizeof(NN_TYPE));
    }

    instance->length = length;

    return instance;

error:
    return NULL;
}

/**
 * Shuffles the elements of a vector instance.
 *
 * @param v A pointer to the vector instance to be shuffled.
 *
 * @return A pointer to the shuffled vector instance.
 */
vector *vector_shuffle(vector *v) {
    size_t size = v->length;
    for(size_t index = 0; index < size; index++) {
        NN_TYPE shuffled_value;
        size_t shuffled;
        
        shuffled = (size_t)nn_random_range(0, size);
        if(shuffled == index) {
            if(index == size - 1) {
                if(index != 0) {
                    shuffled = index - 1;
                }
            } else {
                shuffled = index + 1;
            }
        }
        
        shuffled_value = VECTOR(v, index);
        VECTOR(v, index) = VECTOR(v, shuffled);
        VECTOR(v, shuffled) = shuffled_value;
    }
    
    return v;
}


/**
 * Performs a vector operation on two vectors of the same size.
 *
 * @param v_block A pointer to the first vector.
 * @param w_block A pointer to the second vector or number.
 * @param size The size of the vectors.
 * @param operation The operation to perform on the vectors.
 *
 */
#define VECTOR_TYPE_OPERATION(v_block, w_block, size, operation)               \
    case size: {                                                               \
        v##size##sf *block = (v##size##sf *)v_block;                           \
                                                                               \
        if (NN_DOUBLE >= w->type) {                                            \
            *block = *block operation w->floated;                              \
        } else if (NN_VECTOR == w->type) {                                     \
            *block = *block operation * (v##size##sf *)w_block;                \
        }                                                                      \
        break;                                                                 \
    }

#define VECTOR_METHOD_OPERATION(name, operation)                               \
    vector *vector_##name(vector *v, number *w)                                \
    {                                                                          \
        int   power;                                                           \
        void *values_added_ptr;                                                \
                                                                               \
        VECTOR_CHECK(v);                                                       \
        NUMBER_CHECK(w);                                                       \
                                                                               \
        /* This code uses the __builtin_clz function, which counts the         \
         * number of leading zero bits in an integer, and bit shifting         \
         * to calculate the nearest power of 2 that is greater than or equal   \
         * to the length of the vector. */                                     \
        power = v->length >= 128                                               \
                    ? 128                                                      \
                    : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));       \
                                                                               \
        PRAGMA(omp for schedule(auto))                                         \
        for (size_t index = 0; index < (v)->length; index = index + power) {   \
            void *block_ptr = (NN_TYPE *)(v->number.values) + index;           \
                                                                               \
            if (NN_VECTOR == w->type) {                                        \
                values_added_ptr = (NN_TYPE *)(w->values) + index;             \
                __builtin_prefetch(values_added_ptr + power, 0, 1);            \
            }                                                                  \
            __builtin_prefetch(block_ptr + power, 1, 1);                       \
                                                                               \
            switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {             \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 128,        \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 64,         \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 32,         \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 16,         \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 8,          \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 4,          \
                                      operation);                              \
                VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 2,          \
                                      operation);                              \
            }                                                                  \
        }                                                                      \
                                                                               \
        return v;                                                              \
                                                                               \
    error:                                                                     \
        return NULL;                                                           \
    }

VECTOR_METHOD_OPERATION(addition, +);
VECTOR_METHOD_OPERATION(substraction, -);
VECTOR_METHOD_OPERATION(multiplication, *);
VECTOR_METHOD_OPERATION(division, /);

vector *vector_addition_func(vector *v, number *w)
{
    int   power;
    void *values_added_ptr;

    VECTOR_CHECK(v);
    NUMBER_CHECK(w);

    /* This code uses the __builtin_clz function, which counts the
     * number of leading zero bits in an integer, and bit shifting
     * to calculate the nearest power of 2 that is greater than or equal to
     * the length of the vector. */
    power = v->length >= 128
                ? 128
                : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));

#pragma omp for schedule(auto)
    for (size_t index = 0; index < (v)->length; index = index + power) {
        void *block_ptr = (float *)(v->number.values) + index;

        if (NN_VECTOR == w->type) {
            values_added_ptr = (float *)(w->values) + index;
            __builtin_prefetch(values_added_ptr + power, 0, 1);
        }
        __builtin_prefetch(block_ptr + power, 1, 1);

        switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 128, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 64, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 32, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 16, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 8, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 4, +);
            VECTOR_TYPE_OPERATION(block_ptr, values_added_ptr, 2, +);
        }
    }

    return v;

error:
    return NULL;
}

/**
 * Calculates the dot product of two vectors.
 *
 * @param v A pointer to the first vector.
 * @param w A pointer to the second vector.
 * @return The dot product of the two vectors.
 * @throws If either v or w is NULL, an error is thrown and 0 is returned.
 *
 * This function calculates the dot product of two vectors. The dot product is defined as the sum of the products of the corresponding entries of the two vectors. For example, if v = [1, 2, 3] and w = [4, 5, 6], then the dot product of v and w is 1*4 + 2*5 + 3*6 = 32.
 *
 * Note that this function assumes that the two vectors have the same length. If they do not, the behavior of the function is undefined.
 */
float vector_dot_product(vector *v, vector *w)
{
    VECTOR_CHECK(v);
    VECTOR_CHECK(w);

    float product = 0.0f;

    // #pragma omp parallel for simd reduction (+:product)
    VECTOR_FOREACH(v) { product += VECTOR(v, index) * VECTOR(w, index); }

    return product;

error:
    return 0;
}

/**
 * Macro that defines a vector map operation for a given block size.
 *
 * @param v_block A pointer to the block of memory to perform the operation on.
 * @param size The size of the block of memory.
 * @param operation The operation to perform on each element of the block of memory.
 *
 * This macro defines a vector map operation for a given block size. The operation is performed on each element of the block of memory. The block of memory is assumed to be a vector of type v##size##sf, where v is the vector type and size is the block size.
 */
#define VECTOR_MAP_OPERATION(v_block, size, operation)                         \
    case size: {                                                               \
        v##size##sf *block = (v##size##sf *)v_block;                           \
        for (size_t index = 0; index < size; index++)                          \
            (*block)[index] = operation((*block)[index]);                      \
        break;                                                                 \
    }

vector *vector_map(vector *v, NN_TYPE operation(NN_TYPE))
{
    int   power;
    void *values_added_ptr;

    VECTOR_CHECK(v);

    power = v->length >= 128
                ? 128
                : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));

#pragma omp for schedule(static)
    for (size_t index = 0; index < (v)->length; index = index + power) {
        void *block_ptr = (float *)(v->number.values) + index;

        __builtin_prefetch(block_ptr + power, 1, 1);

        switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {
            VECTOR_MAP_OPERATION(block_ptr, 128, operation);
            VECTOR_MAP_OPERATION(block_ptr, 64, operation);
            VECTOR_MAP_OPERATION(block_ptr, 32, operation);
            VECTOR_MAP_OPERATION(block_ptr, 16, operation);
            VECTOR_MAP_OPERATION(block_ptr, 8, operation);
            VECTOR_MAP_OPERATION(block_ptr, 4, operation);
            VECTOR_MAP_OPERATION(block_ptr, 2, operation);
        }
    }

    return v;

error:
    return NULL;
}

#define VECTOR_MAP_VALUE_OPERATION(v_block, size, operation, value)            \
    case size: {                                                               \
        v##size##sf *block = (v##size##sf *)v_block;                           \
        for (size_t index = 0; index < size; index++)                          \
            (*block)[index] = operation((*block)[index], value);               \
        break;                                                                 \
    }
vector *vector_map_value(vector *v, NN_TYPE operation(NN_TYPE, NN_TYPE *),
                         NN_TYPE *value)
{
    int   power;
    void *values_added_ptr;

    VECTOR_CHECK(v);

    power = v->length >= 128
                ? 128
                : 1 << (sizeof(int) * 8 - __builtin_clz(v->length));

#pragma omp for schedule(static)
    for (size_t index = 0; index < (v)->length; index = index + power) {
        void *block_ptr = (float *)(v->number.values) + index;

        __builtin_prefetch(block_ptr + power, 1, 1);

        switch (__builtin_expect(power, VECTOR_COMMON_SIZE)) {
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 128, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 64, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 32, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 16, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 8, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 4, operation, value);
            VECTOR_MAP_VALUE_OPERATION(block_ptr, 2, operation, value);
        }
    }

    return v;

error:
    return NULL;
}

int vector_index_of(vector *v, float needle)
{
    VECTOR_CHECK(v);

    VECTOR_FOREACH(v)
    {
        if (VECTOR(v, index) == needle) {
            return (int)index;
        }
    }

    return -1;

error:
    return -1;
}

NN_TYPE vector_length(vector *v)
{
    VECTOR_CHECK(v);

    return sqrt(vector_dot_product(v, v));

error:
    return 0;
}

NN_TYPE vector_sum(vector *v)
{
    VECTOR_CHECK(v);

    NN_TYPE sum = 0;

    // #pragma omp parallel for simd reduction (+:sum)
    VECTOR_FOREACH(v) { sum += VECTOR(v, index); }

    return sum;

error:
    return 0;
}

NN_TYPE vector_sum_to(vector *v, size_t to_index)
{
    VECTOR_CHECK(v);

    NN_TYPE sum = 0;

    VECTOR_FOREACH(v)
    {
        sum += VECTOR(v, index);
        if (index == to_index) {
            return sum;
        }
    }

    return sum;

error:
    return 0;
}

NN_TYPE vector_sum_between(vector *v, size_t from_index, size_t to_index)
{
    VECTOR_CHECK(v);

    NN_TYPE sum = 0;

    for (size_t index = from_index; index < to_index; index++) {
        sum += VECTOR(v, index);
    }

    return sum;

error:
    return 0;
}
vector *vector_uniq(vector *instance) {
    size_t size;
    NN_TYPE *uniq;

    VECTOR_CHECK(instance);
    
    size = 0;
    uniq = nn_uniq_numbers(instance->number.values, instance->length, &size);
    CHECK_MEMORY(uniq);

    return vector_from_list(size, uniq);
error:
    return NULL;
}

/**
 * Returns a new vector that is the unit vector of the input vector.
 *
 * @param v A pointer to the vector to be normalized.
 *
 * @return A pointer to a new vector that is the unit vector of the input vector.
 *
 * @throws NULL if the input vector is NULL.
 * @throws NULL if there is an error during the normalization process.
 *
 * This function takes a pointer to a vector and returns a new vector that is the unit vector of the input vector. The input vector is not modified.
 * The unit vector is a vector with the same direction as the input vector, but with a length of 1.
 *
 */
vector *vector_unit(vector *v)
{
    vector *unit_vector;
    VECTOR_CHECK(v);

    number *length = number_create(vector_length(v));
    NUMBER_CHECK(length);

    unit_vector = vector_division(vector_clone(v), length);
    number_delete(length);

    return unit_vector;

error:
    if(length) {
        number_delete(length);
    }
    return NULL;
}

/**
 * Calculates the L-norm of a given vector raised to a specified power.
 *
 * @param v A pointer to the vector to calculate the L-norm of.
 * @param power An integer specifying the power to raise the L-norm to.
 *
 * @return The L-norm of the vector raised to the specified power.
 *
 * @throws A null pointer exception if the vector pointer is null.
 * @throws An invalid argument exception if the power is 0.
 */
NN_TYPE vector_l_norm(vector *v, int power)
{
    VECTOR_CHECK(v);
    CHECK(power, "P = 0 for L_norm");

    float l_norm = 0;

    size_t index = v->length;
    while (index--) {
        l_norm += pow(fabs(VECTOR(v, index)), power);
    }

    l_norm = pow(l_norm, (float)1 / power);

    return l_norm;

error:
    return 0;
}

NN_TYPE vector_max_norm(vector *v)
{
    VECTOR_CHECK(v);

    float max = 0;

    size_t index = v->length;
    while (index--) {
        float value = fabs(VECTOR(v, index));
        if (value > max) {
            max = value;
        }
    }

    return max;

error:
    return 0;
}

size_t vector_max_index(vector *v)
{
    VECTOR_CHECK(v);

    float  max       = 0;
    size_t max_index = 0;

    size_t index = v->length;
    while (index--) {
        float value = fabs(VECTOR(v, index));
        if (value > max) {
            max       = value;
            max_index = index;
        }
    }

    return max_index;

error:
    return 0;
}

/**
 * Calculates the angle between two vectors in degrees.
 *
 * @param v A pointer to the first vector.
 * @param w A pointer to the second vector.
 *
 * @return The angle between the two vectors in degrees.
 * @throws If either v or w is NULL, an error is thrown.
 *
 * @note This function assumes that the vectors are non-zero and non-null.
 * @note The angle is calculated using the dot product and the lengths of the vectors.
 * @note The angle is returned in degrees.
 */
NN_TYPE vector_angle(vector *v, vector *w)
{
    VECTOR_CHECK(v);
    VECTOR_CHECK(w);

    float cosine
        = vector_dot_product(v, w) / (vector_length(v) * vector_length(w));

    float angle_in_degrees = acos(cosine) * 180 / M_PI;

    return angle_in_degrees;

error:
    return -1;
}

/**
 * Determines if two vectors are perpendicular.
 *
 * @param v A pointer to the first vector.
 * @param w A pointer to the second vector.
 *
 * @return 1 if the vectors are perpendicular, 0 otherwise.
 *
 * @throws If either v or w is NULL, an error is thrown.
 *
 * @note This function assumes that the vectors are non-null.
 * @note Two vectors are perpendicular if their dot product is 0.
 */
int vector_is_perpendicular(vector *v, vector *w)
{
    VECTOR_CHECK(v);
    VECTOR_CHECK(w);

    float dot_product = vector_dot_product(v, w);

    return dot_product == 0 ? 1 : 0;

error:
    return -1;
}


int vector_is_equal(vector *v, vector *w)
{
    VECTOR_CHECK(v);
    VECTOR_CHECK(w);

    int is_equal
        = v->length == w->length
          && memcmp(v->number.values, w->number.values, v->length) == 0;

    return is_equal;

error:
    return -1;
}


void vector_print(vector *instance)
{
    VECTOR_CHECK(instance);

    printf("\tVector size = %d\n\t\t[\t", (int)instance->length);

    VECTOR_FOREACH(instance)
    {
        if (index < 5 || index > instance->length - 5) {
            printf("%.4f,\n\t\t\t", VECTOR(instance, index));
        }

        if (index == 6) {
            printf("...,\n\t\t\t");
        }
    }

    printf("\t\t\t]\n");

error:
    return;
}
