# Naive Numbers

`nn` is C library for storing and manipulating different number types.

## Features
* Supports multiple number types including integers, floating-point numbers, vectors, matrices, and tensors.
* Uses union to store different types of numbers.
* Defines custom vector types to perform SIMD operations.
* Provides functions to create, delete, and manipulate numbers.

## API Reference
To use the library, include the header file `nn.h` in your project.
```c
#include <nn.h>
```

`nn` defines functions to work with a numerical type `NN_TYPE`, which can be either `float` or `double`. `vector` is a structure for storing an array of `NN_TYPE` numbers, and the functions operate on `vector` objects.

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
![Vector initialization](docs/images/vector_create.svg)

Stores a vector of numbers.

```c
struct nn_vector {
    struct nn_number number;
    size_t           length;
};
```

### Struct `nn_matrix`
![Matrix initialization](docs/images/matrix_create.svg)

Stores a matrix of numbers.

```c
struct nn_matrix {
    struct nn_number  number;
    size_t            rows;
    size_t            columns;
};
```

### Struct `nn_tensor`

Stores a tensor of numbers.

```c
struct nn_tensor {
    struct nn_number  number;
    size_t            rank;
    size_t           *shape;
};
```

### Typedefs

```c
typedef struct nn_number number;
typedef struct nn_vector vector;
typedef struct nn_matrix matrix;
```

### Macros
Getters:
- `VECTOR(vector, index)`: Returns the `index`-th element of the vector.
- `MATRIX(matrix, row, column)`: Returns the `[row][column]`-th element of the matrix.

Macros for loop for iterating over the elements of the vector and matrix:
- `VECTOR_FOREACH(vector)`
- `MATRIX_FOREACH(matrix)`

### Creating Numbers
| Function | Arguments | Description |
| - | - | - |
| `integer_create` | `unsigned int value` | creates a new `number` object with an integer value. |
| `float_create` | `float value` | creates a new `number` object with a floating-point value. |
| `double_create` | `double value` | creates a new `number` object with a double-precision floating-point value. |

### Creating Vector
| Function | Arguments | Description |
| - | - | - |
| `vector_create` | `size_t length` | creates a new `vector` object with the given length. |
| `vector_from_list` | `size_t length, NN_TYPE data[length]` | creates a new `vector` object with the given length and initializes its values from `data`. |
| `matrix_column_vector` | `matrix *A, size_t column` | returns a new `vector` object of a specified column in the matrix. |
| `vector_clone` | `vector *v` | returns a new `vector` object that is a copy of the given vector `v`. |

### Creating Matrix
| Function | Arguments | Description |
| - | - | - |
| `matrix_create` | `size_t rows, size_t columns` | creates a new `matrix` object with the given size. |
| `matrix_from_list` | `size_t rows, size_t columns, NN_TYPE data[length]` | creates a new `matrix` object with the given length and initializes its values from `data`. |
| `matrix_from_vector` | `vector *v, size_t columns` | creates a new `matrix` object with elements from a vector, with the specified number of columns. |
| `matrix_from_vectors` | `vector **vectors, size_t rows, size_t columns` | converts multiple vectors to a matrix, with the specified number of rows and columns. |
| `matrix_identity` | `size_t size` | returns a square identity matrix of the specified size. |
| `matrix_clone` | `matrix *A` | returns a new `matrix` object that is a copy of the given matrix `A`. |

### Deleting Number, Vector, Matrix
| Function | Arguments | Description |
| - | - | - |
| `number_delete` | `number *n` | deletes a `number` object. This function should be used for both scalar numbers and vectors. |

### Vector Properties
| Function | Arguments | Description |
| - | - | - |
| `vector_length` | `vector *v` | returns the length of the vector `v`. |
| `vector_unit` | `vector *v` | returns a new `vector` object that is the unit vector of the given vector `v`. |
| `vector_l_norm` | `vector *v, int power` | calculates the L-norm of the given vector `v` with the specified power. The L-norm is a measure of the magnitude of a vector and is defined as the sum of the absolute values of the elements raised to the specified power. |
| `vector_max_norm` | `vector *v` | returns the maximum norm of the given vector `v`, which is defined as the maximum absolute value of the elements in the vector. |
| `vector_max_index` | `vector *v` | returns the index of the element with the maximum absolute value in the given vector `v` |
| `vector_index_of` | `vector *v, float needle` | returns the index of the first occurrence of `needle` in the `vector` `v`. If not found, the function returns `-1`. |
| `vector_sum` | `vector *v` | returns the sum of all elements in the given vector `v`. |
| `vector_sum_to` | `vector *v, size_t to_index` | returns the sum of all elements from the start of the vector `v` up to the specified `to_index`. |
| `vector_sum_between` | `vector *v, size_t from_index, size_t to_index` | returns the sum of all elements in the vector `v` between the specified indices `from_index` and `to_index` (inclusive). |

### Matrix Properties
| Function | Arguments | Description |
| - | - | - |
| `matrix_sum` | `matrix *A` | returns the sum of all elements in the given matrix `A`. |
| `matrix_trace` | `matrix *A` | returns the trace of the matrix `A`, which is defined as the sum of the elements on the main diagonal.. |
| `matrix_frobenius_norm` | `matrix *A` | returns the Frobenius norm of a matrix. |
| `matrix_frobenius_norm_by_trace` | `matrix *A` | returns the Frobenius norm of a matrix, computed using the trace of the matrix. |

### Vectors Operations
| Function | Arguments | Description |
| - | - | - |
| `vector_seed` | `vector *v` | initializes all elements of the vector with the given default value or random in case of 0 default value. |
| `vector_reshape` | `vector *instance, size_t length` | returns a new `vector` object with the given length, reshaped from the given vector `instance`. |
| `vector_dot_product` | `vector *v, vector *w` | calculates the dot product of two vectors `v` and `w`. The dot product is a scalar value calculated by multiplying corresponding elements of the vectors and summing the results. The function returns the calculated dot product as a `NN_TYPE` value. |

This functions updates the elements of vector `v` by applying some operation to each element, and then returns a reference to the updated vector:
| Function | Arguments | Description |
| - | - | - |
| `vector_map` | `vector *v, NN_TYPE operation(NN_TYPE)` | |
| `vector_map_value` | `vector *v, NN_TYPE operation(NN_TYPE, NN_TYPE*), NN_TYPE* value` | applies a function `operation` to each element of the vector `v` with a specific `value` |

This functions updates the elements of vector `v` by adding/substracting/multiplicating/divising the value of `w` (scalar or vector) to each element, and then returns a reference to the updated vector:
| Function | Arguments |
| - | - |
| `vector_addition` | `vector *v, number *w` |
| `vector_substraction` | `vector *v, number *w` |
| `vector_multiplication` | `vector *v, number *w` |
| `vector_division` | `vector *v, number *w` |

### Matrix operations
| Function | Arguments | Description |
| - | - | - |
| `matrix_reshape` | `matrix *instance, size_t length` | reshapes the input matrix to have the specified number of rows and columns. |
| `matrix_dot_product` | `matrix *A, matrix *B` | calculates the matrix product of two matrices `A` and `B` . |
| `matrix_transpose` | `matrix *instance` | transpose of a given matrix. |
| `vector_transformation_by_matrix` | `matrix *A, vector *x` | transforms a vector by multiplying it with a matrix. |

This functions updates the elements of matrix `A` by applying some operation to each element, and then returns a reference to the updated matrix:
| Function | Arguments | Description |
| - | - | - |
| `matrix_map` | `matrix *A, NN_TYPE operation(NN_TYPE)` | |
| `matrix_map_value` | `matrix *A, NN_TYPE operation(NN_TYPE, NN_TYPE*), NN_TYPE* value` | applies a function `operation` to each element of the matrix `A` with a specific `value` |

This functions updates the elements of matrix `A` by adding/substracting/multiplicating/divising the value of `B` (scalar or matrix) to each element, and then returns a reference to the updated matrix:
| Function | Arguments |
| - | - |
| `matrix_addition` | `matrix *A, number *B` |
| `matrix_substraction` | `matrix *A, number *B` |
| `matrix_multiplication` | `matrix *A, number *B` |
| `matrix_division` | `matrix *A, number *B` |

### Vectors Relations
| Function | Arguments | Description |
| - | - | - |
| `vector_angle` | `vector *v, vector *w` | calculates the angle between the two given vectors `v` and `w`. This is defined as the acute angle between the two vectors and is expressed in radians. |
| `vector_is_perpendicular` | `vector *v, vector *w` | checks if the two vectors `v` and `w` are perpendicular. It returns 1 if they are perpendicular and 0 otherwise. |
| `vector_is_equal` | `vector *v, vector *w` | checks if the two vectors `v` and `w` are equal. It returns 1 if they are equal and 0 otherwise. |

### Matrix Relations
| Function | Arguments | Description |
| - | - | - |
| `matrix_is_equal` | `matrix *A, matrix *B` | checks if the two matricies `A` and `B` are equal. It returns 1 if they are equal and 0 otherwise. |

