# Naive Numbers (nn) Library Tutorial

## Philosophy

Naive Numbers (nn) is a C library designed for numerical computing with a focus on simplicity, performance, and flexibility. The library follows several key philosophical principles:

### 1. Type Unification

The library unifies different numerical types (scalars, vectors, matrices, tensors) under a common `number` structure. This allows for consistent handling of different mathematical objects and enables polymorphic operations.

### 2. Performance-Oriented Design

While maintaining a clean API, nn is designed with performance in mind:
- Uses SIMD instructions for vector operations
- Employs OpenMP for parallel processing
- Optimizes memory access patterns
- Provides specialized implementations for common operations

### 3. Composable Operations

Operations in nn are designed to be composable, allowing you to build complex computations from simple building blocks. Many functions return the modified object, enabling method chaining.

#### Operation Chaining

One of the most powerful features of the nn library is the ability to chain operations together. Since most transformation functions modify objects in-place and return the modified object, you can chain multiple operations in a single expression:

```c
// Create a vector and perform multiple operations in a single chain
vector *v = vector_create(5);
vector_seed(v, 1.0);
vector *result = vector_addition(
                   vector_multiplication(
                     vector_map(v, sqrt),
                     number_create(2.0)
                   ),
                   float_create(1.0)
                 );
// This chain: takes v, applies sqrt to each element, multiplies by 2, adds 1

// Matrix operation chaining
matrix *m = matrix_create(3, 3);
matrix_seed(m, 1.0);
matrix *processed = matrix_map(
                      matrix_multiplication(
                        m, 
                        matrix_transpose(matrix_clone(m))
                      ),
                      sqrt
                    );
// This chain: multiplies m by its transpose, then takes sqrt of each element
```

This chaining approach reduces the need for temporary variables and makes the code more concise and readable. It's particularly useful for complex mathematical operations that would otherwise require multiple intermediate steps.

### 4. Consistent Memory Management

The library follows a consistent pattern for memory management:
- Creation functions (`*_create`, `*_from_*`) allocate memory
- The user is responsible for freeing memory with `number_delete()`
- Transformation functions typically modify objects in-place

## Core Components

### Number

The fundamental type that represents any numerical value:

```c
// Create basic number types
number *n = number_create(3.14);
number *i = integer_create(42);
number *f = float_create(2.718);

// Always clean up when done
number_delete(n);
number_delete(i);
number_delete(f);
```

### Vector

Vectors represent one-dimensional arrays of numbers:

```c
// Create a vector with 5 elements
vector *v = vector_create(5);

// Initialize with a specific value
vector_seed(v, 1.0);

// Create from existing values
vector *w = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});

// Vector operations
vector_addition(v, (number *)w);      // v = v + w
vector_subtraction(v, (number *)w);   // v = v - w
vector_multiplication(v, (number *)w); // v = v * w
vector_division(v, (number *)w);      // v = v / w

// Vector properties
NN_TYPE length = vector_length(v);
NN_TYPE dot = vector_dot_product(v, w);
vector *unit = vector_unit(v);

// Clean up
number_delete(v);
number_delete(w);
number_delete(unit);
```

### Matrix

Matrices represent two-dimensional arrays of numbers:

```c
// Create a 3x3 matrix
matrix *m = matrix_create(3, 3);

// Initialize with a specific value
matrix_seed(m, 1.0);

// Create from existing values
matrix *n = matrix_create_from_list(2, 3, 
    (NN_TYPE[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

// Create an identity matrix
matrix *identity = matrix_identity(3, 1.0);

// Matrix operations
matrix *transposed = matrix_transpose(n);
matrix *product = matrix_multiplication(m, n);

// Matrix properties
NN_TYPE sum = matrix_sum(m);
NN_TYPE trace = matrix_trace(m);
NN_TYPE norm = matrix_frobenius_norm(m);

// Clean up
number_delete(m);
number_delete(n);
number_delete(identity);
number_delete(transposed);
number_delete(product);
```

### Probability

The library provides tools for probability and statistics:

```c
// Create a sample matrix
matrix *samples = matrix_create_from_list(3, 2, 
    (NN_TYPE[]){0.8, 0.2, 0.2, 0.4, 0.5, 0.7});

// Define field names
char *fields[] = {"field1", "field2"};

// Create probability space
probability *p = probability_from_matrix(samples, fields);

// Probability calculations
NN_TYPE mass = probability_mass_of(p, "field1", 0.8);
NN_TYPE expected = probability_expected_value(p, "field2");
NN_TYPE variance = probability_variance(p, "field1");
NN_TYPE covariance = probability_covariance(p, "field1", "field2");
NN_TYPE correlation = probability_correlation(p, "field1", "field2");

// Clean up
number_delete(samples);
probability_delete(p);
```

## Best Practices

1. **Always check return values**: Most functions return NULL or an error code on failure.
2. **Free memory**: Use `number_delete()` for all created objects.
3. **Use the macros**: Leverage `VECTOR()`, `MATRIX()`, and loop macros like `VECTOR_FOREACH()`.
4. **Understand in-place operations**: Many functions modify objects in-place and return the modified object.
5. **Leverage SIMD operations**: For performance-critical code, use the vector operations that utilize SIMD.

## Advanced Usage

### Custom Vector Operations

You can apply custom functions to vectors:

```c
// Define a custom function
NN_TYPE square(NN_TYPE x) {
    return x * x;
}

// Apply to a vector
vector *v = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
vector_map(v, square);
```

### Matrix Decomposition

```c
matrix *A = matrix_create_from_list(3, 3, /* values */);
matrix *L, *U;
int rank = matrix_lu_decomposition(A, &L, &U);
```

## Practical Examples

### Complex Vector Operations

Here's an example of how to use operation chaining for complex vector manipulations:

```c
// Normalize a vector, scale it, and calculate its dot product with another vector
vector *v1 = vector_from_list(3, (NN_TYPE[]){3.0, 4.0, 5.0});
vector *v2 = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});

// Create a unit vector from v1, scale it by 2, and add v2
vector *result = vector_addition(
                   vector_multiplication(
                     vector_unit(v1),
                     float_create(2.0)
                   ),
                   (number *)v2
                 );

// Calculate dot product with original vector
NN_TYPE dot = vector_dot_product(result, v1);

// Clean up
number_delete(v1);
number_delete(v2);
number_delete(result);
```

### Matrix Transformations

Here's how to chain operations for matrix transformations:

```c
// Create a matrix, calculate its transpose, multiply by original, and extract diagonal
matrix *m = matrix_create_from_list(2, 2, (NN_TYPE[]){1.0, 2.0, 3.0, 4.0});

// Create m * m^T (a symmetric matrix)
matrix *symmetric = matrix_multiplication(m, matrix_transpose(matrix_clone(m)));

// Extract diagonal and normalize
vector *diagonal = matrix_column_vector(symmetric, 0);
vector *normalized = vector_unit(diagonal);

// Clean up
number_delete(m);
number_delete(symmetric);
number_delete(diagonal);
number_delete(normalized);
```

## Conclusion

The Naive Numbers library provides a powerful yet simple framework for numerical computing in C. By following its design philosophy and leveraging its optimized operations, you can build efficient numerical applications with clean, maintainable code. The ability to chain operations makes complex mathematical expressions more readable and reduces the need for temporary variables, leading to more elegant code.
