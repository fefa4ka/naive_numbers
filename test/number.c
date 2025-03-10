/**
 * Test for number operations in the Naive Numbers library
 *
 * This test verifies the basic functionality of number operations,
 * including creation, deletion, and reference counting.
 */

#include <math.h>
#include <nn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Simple test assertion macro
#define test_assert(test, message, ...)                                        \
    if (!(test)) {                                                             \
        printf("ERROR: " message "\n", ##__VA_ARGS__);                         \
        return 1;                                                              \
    } else {                                                                   \
        printf("OK: " message "\n", ##__VA_ARGS__);                            \
    }

// Test number creation and basic properties
int test_number_creation()
{
    printf("\n=== Testing Number Creation ===\n");

    // Create different types of numbers
    number *n = number_create(3.14);
    number *i = integer_create(42);
    number *f = float_create(2.718);
    number *d = double_create(1.618);

    // Verify types
    test_assert(n->type == NN_TYPE_ENUM, "number_create sets correct type");
    test_assert(i->type == NN_INTEGER, "integer_create sets correct type");
    test_assert(f->type == NN_FLOAT, "float_create sets correct type");
    test_assert(d->type == NN_DOUBLE, "double_create sets correct type");

    // Verify values
    test_assert(fabs(n->floated - 3.14) < 0.0001,
                "number_create sets correct value");
    test_assert(i->integer == 42, "integer_create sets correct value");
    test_assert(fabs(f->floated - 2.718) < 0.0001,
                "float_create sets correct value");
    test_assert(fabs(d->doubled - 1.618) < 0.0001,
                "double_create sets correct value");

    // Verify initial reference count
    test_assert(n->ref_count == 1, "Initial reference count is 1");
    test_assert(i->ref_count == 1, "Initial reference count is 1");
    test_assert(f->ref_count == 1, "Initial reference count is 1");
    test_assert(d->ref_count == 1, "Initial reference count is 1");

    // Clean up
    number_delete(n);
    number_delete(i);
    number_delete(f);
    number_delete(d);

    return 0;
}

// Test reference counting
int test_reference_counting()
{
    printf("\n=== Testing Reference Counting ===\n");

    // Create a number
    number *n = number_create(3.14);
    test_assert(n->ref_count == 1, "Initial reference count is 1");

    // Increment reference count
    number *n_ref = number_ref(n);
    test_assert(n->ref_count == 2, "Reference count incremented to 2");
    test_assert(n == n_ref, "number_ref returns the same pointer");

    // Increment again
    number_ref(n);
    test_assert(n->ref_count == 3, "Reference count incremented to 3");

    // Decrement reference count
    number_unref(n);
    test_assert(n->ref_count == 2, "Reference count decremented to 2");

    // Decrement again
    number_unref(n);
    test_assert(n->ref_count == 1, "Reference count decremented to 1");

    // Final decrement should free the memory
    number_unref(n);
    // Cannot test n->ref_count here as n is freed

    return 0;
}

// Test reference counting with vectors
int test_vector_reference_counting()
{
    printf("\n=== Testing Vector Reference Counting ===\n");

    // Create vectors
    vector *v1 = vector_create(3);
    vector_seed(v1, 1.0);
    vector *v2 = vector_create(3);
    vector_seed(v2, 2.0);

    // Test initial reference counts
    test_assert(v1->number.ref_count == 1,
                "Vector initial reference count is 1");
    test_assert(v2->number.ref_count == 1,
                "Vector initial reference count is 1");

    // Test reference counting in vector operations
    vector *result = vector_addition(
        vector_multiplication(v1, 
            float_create(2.0)),
        (number *)v2);

    test_assert(v1->number.ref_count == 1,
                "v1 reference count is correct after operations");

    // v2 should be unreferenced by vector_addition

    // result should be the same as v1 (operations modify in place)
    test_assert(result == v1,
                "Result is the same as v1 (operations modify in place)");

    // Clean up
    number_delete(v1); // This also cleans up result since they're the same

    return 0;
}

// Test NULL handling in reference counting
int test_null_reference_counting()
{
    printf("\n=== Testing NULL Reference Counting ===\n");

    // Test number_ref with NULL
    number *n_null = NULL;
    number *n_ref  = number_ref(n_null);
    test_assert(n_ref == NULL, "number_ref handles NULL correctly");

    // Test number_unref with NULL
    number_unref(n_null); // Should not crash
    printf("OK: number_unref handles NULL correctly\n");

    return 0;
}

// Test memory management with complex operations
int test_complex_memory_management()
{
    printf("\n=== Testing Complex Memory Management ===\n");

    // Create a matrix
    matrix *m = matrix_create(3, 3);
    matrix_seed(m, 1.0);

    // Create a vector
    vector *v = vector_create(3);
    vector_seed(v, 2.0);

    // Perform complex operations
    vector *result = vector_transformation_by_matrix(m, v);

    // v should be unreferenced by vector_transformation_by_matrix

    // Clean up
    number_delete(m);
    number_delete(result);

    return 0;
}

int main()
{
    printf("=== Naive Numbers Number Test ===\n");

    // Seed random number generator
    srand(time(NULL));

    // Run tests
    int result = 0;
    result |= test_number_creation();
    result |= test_reference_counting();
    result |= test_vector_reference_counting();
    result |= test_null_reference_counting();
    result |= test_complex_memory_management();

    if (result == 0) {
        printf("\nAll number tests passed successfully!\n");
    } else {
        printf("\nSome tests failed!\n");
    }

    return result;
}
