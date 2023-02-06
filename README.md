# Naive Numbers

`nn` is C library for storing and manipulating different number types.

## Features
* Supports multiple number types including integers, floating-point numbers, vectors, matrices, and tensors.
* Uses union to store different types of numbers.
* Defines custom vector types to perform SIMD operations.
* Provides functions to create, delete, and manipulate numbers.

## Usage
To use the library, include the header file `nn.h` in your project.
```c
#include <nn.h>
```

`nn` defines functions to work with a numerical type `NN_TYPE`, which can be either `float` or `double`. `vector` is a structure for storing an array of `NN_TYPE` numbers, and the functions operate on `vector` objects.

## Creating Numbers
- `number *integer_create(unsigned int value)`: creates a new `number` object with an integer value.
- `number *float_create(float value)`: creates a new `number` object with a floating-point value.
- `number *double_create(double value)`: creates a new `number` object with a double-precision floating-point value.
- `int number_delete(number *n)`: deletes a `number` object. This function should be used for both scalar numbers and vectors.

## Creating Vectors
- `vector *vector_create(int length)`: creates a new `vector` object with the given length.
- `vector *vector_from_list(int length, NN_TYPE data[length])`: creates a new `vector` object with the given length and initializes its values from `data`.
- `vector *vector_clone(vector *v)`: returns a new `vector` object that is a copy of the given vector `v`.

## Deleting Numbers and Vectors
- `int number_delete(number *n)`: deletes a `number` object. This function should be used for both scalar numbers and vectors.

## Vector Properties
- `float vector_length(vector *v)`: returns the length of the vector `v`.
- `vector *vector_unit(vector *v)`: returns a new `vector` object that is the unit vector of the given vector `v`.
- `NN_TYPE vector_l_norm(vector *v, int power)`: calculates the L-norm of the given vector `v` with the specified power. The L-norm is a measure of the magnitude of a vector and is defined as the sum of the absolute values of the elements raised to the specified power.
- `NN_TYPE vector_max_norm(vector *v)`: returns the maximum norm of the given vector `v`, which is defined as the maximum absolute value of the elements in the vector.
- `size_t vector_max_index(vector *v)`: returns the index of the element with the maximum absolute value in the given vector `v`.
- `int vector_index_of(vector *v, float needle)` returns the index of the first occurrence of `needle` in the `vector` `v`. If not found, the function returns `-1`.
- `NN_TYPE vector_sum(vector *v)`: returns the sum of all elements in the given vector `v`.
- `NN_TYPE vector_sum_to(vector *v, size_t to_index)`: returns the sum of all elements from the start of the vector `v` up to the specified `to_index`.
- `NN_TYPE vector_sum_between(vector *v, size_t from_index, size_t to_index)`: returns the sum of all elements in the vector `v` between the specified indices `from_index` and `to_index` (inclusive).

## Vectors Operations
- `vector *vector_reshape(vector *instance, size_t length)`: returns a new `vector` object with the given length, reshaped from the given vector `instance`.
- `NN_TYPE vector_dot_product(vector *v, vector *w)` calculates the dot product of two vectors `v` and `w`. The dot product is a scalar value calculated by multiplying corresponding elements of the vectors and summing the results. The function returns the calculated dot product as a `NN_TYPE` value.

This functions updates the elements of vector `v` by applying some operation to each element, and then returns a reference to the updated vector:
- `vector *vector_map(vector *v, NN_TYPE operation(NN_TYPE))`
- `vector *vector_map_value(vector *v, NN_TYPE operation(NN_TYPE, NN_TYPE*), NN_TYPE* value)`: applies a function `operation` to each element of the vector `v` with a specific `value`

This functions updates the elements of vector `v` by adding/substracting/multiplicating/divising the value of `w` (scalar or vector) to each element, and then returns a reference to the updated vector:
- `vector *vector_addition(vector *v, number *w)`
- `vector *vector_substraction(vector *v, number *w)`
- `vector *vector_multiplication(vector *v, number *w)`
- `vector *vector_division(vector *v, number *w)`

## Vectors Relations
- `NN_TYPE vector_angle(vector *v, vector *w)`: calculates the angle between the two given vectors `v` and `w`. This is defined as the acute angle between the two vectors and is expressed in radians.
- `int vector_is_perpendicular(vector *v, vector *w)`: checks if the two vectors `v` and `w` are perpendicular. It returns 1 if they are perpendicular and 0 otherwise.
- `int vector_is_equal(vector *v, vector *w)`: checks if the two vectors `v` and `w` are equal. It returns 1 if they are equal and 0 otherwise.

## API Reference
### Enumeration `nn_type`
Enumerates the different types of numbers that can be stored.
```c
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
```

### Struct nn_number
Stores any type of number.

```c
struct nn_number {
    union {
        unsigned int integer;
        float        floated;
        double       doubled;
        void        *values;
    };
    enum nn_type     type;
};
```

### Struct `nn_vector`

Stores a vector of numbers.

```c
struct nn_vector {
    struct nn_number number;
    size_t            length;
};
```

### Struct `nn_matrix`

Stores a matrix of numbers.

```c
struct nn_matrix {
    struct nn_number number;
    size_t            rows;
    size_t            columns;
};
```

### Struct `nn_tensor`

Stores a tensor of numbers.

```c
struct nn_tensor {
    struct nn_number number;
    size_t            rank;
    size_t           *shape;
};
```

### Typedefs

```c
typedef struct nn_number number;
typedef struct nn_vector vector;
```

### Macros
-   `VECTOR(vector, index)`: Returns the `index`-th element of the vector.
-   `VECTOR_FOREACH(vector)`: A for loop for iterating over the elements of the vector.
