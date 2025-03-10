/**
 * Test for vector_clone method in the Naive Numbers library
 *
 * This test verifies that vector_clone correctly creates a deep copy
 * of a vector with all properties preserved.
 */

#include <nn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Simple test assertion macro
#define test_assert(test, message, ...)                                        \
    if (!(test)) {                                                             \
        printf("ERROR: " message "\n", ##__VA_ARGS__);                         \
        return 1;                                                              \
    } else {                                                                   \
        printf("OK: " message "\n", ##__VA_ARGS__);                            \
    }

// Test basic vector cloning
int test_basic_vector_clone() {
    printf("\n=== Testing Basic Vector Clone ===\n");
    
    // Create original vector
    vector *original = vector_create(5);
    test_assert(original != NULL, "Original vector created successfully");
    
    // Fill with known values
    for (size_t i = 0; i < original->length; i++) {
        VECTOR(original, i) = i * 2.5;
    }
    
    // Clone the vector
    vector *clone = vector_clone(original);
    test_assert(clone != NULL, "Clone vector created successfully");
    
    // Verify properties
    test_assert(clone->length == original->length, 
                "Clone has same length as original (%zu)", original->length);
    test_assert(clone->number.type == original->number.type, 
                "Clone has same type as original");
    test_assert(clone->number.ref_count == 1, 
                "Clone has initial ref_count of 1");
    
    // Verify values
    for (size_t i = 0; i < original->length; i++) {
        test_assert(fabs(VECTOR(clone, i) - VECTOR(original, i)) < NN_TYPE_EPSILON, 
                    "Clone value at index %zu matches original (%f)", 
                    i, VECTOR(original, i));
    }
    
    // Verify it's a deep copy (different memory addresses)
    test_assert(clone != original, 
                "Clone is a different object than original");
    test_assert(clone->number.values != original->number.values, 
                "Clone values array is different from original");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)clone);
    
    return 0;
}

// Test cloning with subsequent modifications
int test_clone_with_modifications() {
    printf("\n=== Testing Clone with Modifications ===\n");
    
    // Create and initialize original vector
    vector *original = vector_from_list(4, (NN_TYPE[]){1.1, 2.2, 3.3, 4.4});
    test_assert(original != NULL, "Original vector created successfully");
    
    // Clone the vector
    vector *clone = vector_clone(original);
    test_assert(clone != NULL, "Clone vector created successfully");
    
    // Modify the clone
    VECTOR(clone, 0) = 5.5;
    VECTOR(clone, 2) = 7.7;
    
    // Verify original is unchanged
    test_assert(fabs(VECTOR(original, 0) - 1.1) < NN_TYPE_EPSILON, 
                "Original value at index 0 unchanged after clone modification");
    test_assert(fabs(VECTOR(original, 2) - 3.3) < NN_TYPE_EPSILON, 
                "Original value at index 2 unchanged after clone modification");
    
    // Modify the original
    VECTOR(original, 1) = 6.6;
    VECTOR(original, 3) = 8.8;
    
    // Verify clone is unchanged
    test_assert(fabs(VECTOR(clone, 1) - 2.2) < NN_TYPE_EPSILON, 
                "Clone value at index 1 unchanged after original modification");
    test_assert(fabs(VECTOR(clone, 3) - 4.4) < NN_TYPE_EPSILON, 
                "Clone value at index 3 unchanged after original modification");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)clone);
    
    return 0;
}

// Test cloning with edge cases
int test_clone_edge_cases() {
    printf("\n=== Testing Clone Edge Cases ===\n");
    
    // Test with empty vector (length 0)
    vector *empty = vector_create(0);
    test_assert(empty != NULL, "Empty vector created successfully");
    
    vector *empty_clone = vector_clone(empty);
    test_assert(empty_clone != NULL, "Empty vector cloned successfully");
    test_assert(empty_clone->length == 0, "Empty clone has length 0");
    
    // Test with very large vector
    size_t large_size = 1000000; // 1 million elements
    vector *large = vector_create(large_size);
    test_assert(large != NULL, "Large vector created successfully");
    
    // Set some sentinel values
    VECTOR(large, 0) = 42.0;
    VECTOR(large, large_size/2) = 43.0;
    VECTOR(large, large_size-1) = 44.0;
    
    vector *large_clone = vector_clone(large);
    test_assert(large_clone != NULL, "Large vector cloned successfully");
    test_assert(large_clone->length == large_size, 
                "Large clone has correct length");
    
    // Check sentinel values
    test_assert(fabs(VECTOR(large_clone, 0) - 42.0) < NN_TYPE_EPSILON, 
                "First sentinel value preserved in large clone");
    test_assert(fabs(VECTOR(large_clone, large_size/2) - 43.0) < NN_TYPE_EPSILON, 
                "Middle sentinel value preserved in large clone");
    test_assert(fabs(VECTOR(large_clone, large_size-1) - 44.0) < NN_TYPE_EPSILON, 
                "Last sentinel value preserved in large clone");
    
    // Test with special values
    vector *special = vector_from_list(4, (NN_TYPE[]){
        INFINITY, -INFINITY, NAN, 0.0
    });
    test_assert(special != NULL, "Special values vector created successfully");
    
    vector *special_clone = vector_clone(special);
    test_assert(special_clone != NULL, "Special values vector cloned successfully");
    
    // Check special values (note: can't directly compare NaN)
    test_assert(isinf(VECTOR(special_clone, 0)) && VECTOR(special_clone, 0) > 0, 
                "INFINITY preserved in clone");
    test_assert(isinf(VECTOR(special_clone, 1)) && VECTOR(special_clone, 1) < 0, 
                "-INFINITY preserved in clone");
    test_assert(isnan(VECTOR(special_clone, 2)), 
                "NAN preserved in clone");
    test_assert(fabs(VECTOR(special_clone, 3)) < NN_TYPE_EPSILON, 
                "Zero preserved in clone");
    
    // Clean up
    number_delete((number*)empty);
    number_delete((number*)empty_clone);
    number_delete((number*)large);
    number_delete((number*)large_clone);
    number_delete((number*)special);
    number_delete((number*)special_clone);
    
    return 0;
}

// Test cloning with operations
int test_clone_with_operations() {
    printf("\n=== Testing Clone with Operations ===\n");
    
    // Create original vector
    vector *original = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
    test_assert(original != NULL, "Original vector created successfully");
    
    // Clone and perform operations on the clone
    vector *clone = vector_clone(original);
    test_assert(clone != NULL, "Clone created successfully");
    
    // Perform operations on clone
    vector_addition(clone, float_create(1.0));
    
    // Verify original is unchanged
    test_assert(fabs(VECTOR(original, 0) - 1.0) < NN_TYPE_EPSILON, 
                "Original unchanged after operations on clone");
    test_assert(fabs(VECTOR(original, 1) - 2.0) < NN_TYPE_EPSILON, 
                "Original unchanged after operations on clone");
    test_assert(fabs(VECTOR(original, 2) - 3.0) < NN_TYPE_EPSILON, 
                "Original unchanged after operations on clone");
    
    // Verify clone was modified
    test_assert(fabs(VECTOR(clone, 0) - 2.0) < NN_TYPE_EPSILON, 
                "Clone correctly modified by operation");
    test_assert(fabs(VECTOR(clone, 1) - 3.0) < NN_TYPE_EPSILON, 
                "Clone correctly modified by operation");
    test_assert(fabs(VECTOR(clone, 2) - 4.0) < NN_TYPE_EPSILON, 
                "Clone correctly modified by operation");
    
    // Test in chained operations
    vector *result = vector_addition(
                       vector_multiplication(
                         vector_clone(original),  // Clone used in chain
                         float_create(2.0)
                       ),
                       (number*)clone
                     );
    
    // Verify result
    test_assert(fabs(VECTOR(result, 0) - (1.0*2.0 + 2.0)) < NN_TYPE_EPSILON, 
                "Chained operation with clone produced correct result");
    test_assert(fabs(VECTOR(result, 1) - (2.0*2.0 + 3.0)) < NN_TYPE_EPSILON, 
                "Chained operation with clone produced correct result");
    test_assert(fabs(VECTOR(result, 2) - (3.0*2.0 + 4.0)) < NN_TYPE_EPSILON, 
                "Chained operation with clone produced correct result");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)clone);
    number_delete((number*)result);
    
    return 0;
}

// Test stress test with multiple clones
int test_stress_multiple_clones() {
    printf("\n=== Stress Testing with Multiple Clones ===\n");
    
    // Create original vector
    vector *original = vector_create(10);
    test_assert(original != NULL, "Original vector created successfully");
    vector_seed(original, 1.0);
    
    // Create multiple clones
    const int NUM_CLONES = 100;
    vector *clones[NUM_CLONES];
    
    for (int i = 0; i < NUM_CLONES; i++) {
        clones[i] = vector_clone(original);
        test_assert(clones[i] != NULL, "Clone %d created successfully", i);
        
        // Modify each clone differently
        for (size_t j = 0; j < clones[i]->length; j++) {
            VECTOR(clones[i], j) += i * 0.1;
        }
    }
    
    // Verify all clones are independent
    for (int i = 0; i < NUM_CLONES; i++) {
        // Check against original
        test_assert(fabs(VECTOR(clones[i], 0) - (1.0 + i * 0.1)) < NN_TYPE_EPSILON, 
                    "Clone %d correctly modified independently", i);
        
        // Check against other clones
        for (int j = i + 1; j < NUM_CLONES; j++) {
            test_assert(fabs(VECTOR(clones[i], 0) - VECTOR(clones[j], 0)) > NN_TYPE_EPSILON, 
                        "Clones %d and %d have different values", i, j);
        }
    }
    
    // Clean up
    number_delete((number*)original);
    for (int i = 0; i < NUM_CLONES; i++) {
        number_delete((number*)clones[i]);
    }
    
    return 0;
}

int main() {
    printf("=== Naive Numbers Vector Clone Test ===\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run tests
    int result = 0;
    result |= test_basic_vector_clone();
    result |= test_clone_with_modifications();
    result |= test_clone_edge_cases();
    result |= test_clone_with_operations();
    result |= test_stress_multiple_clones();
    
    if (result == 0) {
        printf("\nAll vector_clone tests passed successfully!\n");
    } else {
        printf("\nSome tests failed!\n");
    }
    
    return result;
}
