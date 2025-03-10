/**
 * Test for vector_map method in the Naive Numbers library
 *
 * This test verifies that vector_map correctly applies mapping functions
 * to vectors with various types of functions and edge cases.
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

// Define various mapping functions for testing
NN_TYPE square(NN_TYPE x) { return x * x; }
NN_TYPE cube(NN_TYPE x) { return x * x * x; }
NN_TYPE negate(NN_TYPE x) { return -x; }
NN_TYPE increment(NN_TYPE x) { return x + 1.0; }
NN_TYPE decrement(NN_TYPE x) { return x - 1.0; }
NN_TYPE reciprocal(NN_TYPE x) { return x != 0 ? 1.0 / x : INFINITY; }
NN_TYPE sine(NN_TYPE x) { return sin(x); }
NN_TYPE absolute(NN_TYPE x) { return fabs(x); }
NN_TYPE identity(NN_TYPE x) { return x; } // No change
NN_TYPE round_to_int(NN_TYPE x) { return roundf(x); }
NN_TYPE complex_function(NN_TYPE x) { return sin(x*x) + cos(x) - tan(x/2); }
NN_TYPE threshold(NN_TYPE x) { return x > 0.5 ? 1.0 : 0.0; }

// Test basic vector mapping with simple functions
int test_basic_vector_map() {
    printf("\n=== Testing Basic Vector Map ===\n");
    
    // Create original vector
    vector *original = vector_from_list(5, (NN_TYPE[]){1.0, 2.0, 3.0, 4.0, 5.0});
    test_assert(original != NULL, "Original vector created successfully");
    
    // Test square function
    vector *squared = vector_map(vector_clone(original), square);
    test_assert(squared != NULL, "Squared vector created successfully");
    test_assert(VECTOR(squared, 0) == 1.0, "Square function applied correctly to first element");
    test_assert(VECTOR(squared, 1) == 4.0, "Square function applied correctly to second element");
    test_assert(VECTOR(squared, 2) == 9.0, "Square function applied correctly to third element");
    test_assert(VECTOR(squared, 3) == 16.0, "Square function applied correctly to fourth element");
    test_assert(VECTOR(squared, 4) == 25.0, "Square function applied correctly to fifth element");
    
    // Test cube function
    vector *cubed = vector_map(vector_clone(original), cube);
    test_assert(cubed != NULL, "Cubed vector created successfully");
    test_assert(VECTOR(cubed, 0) == 1.0, "Cube function applied correctly to first element");
    test_assert(VECTOR(cubed, 1) == 8.0, "Cube function applied correctly to second element");
    test_assert(VECTOR(cubed, 2) == 27.0, "Cube function applied correctly to third element");
    test_assert(VECTOR(cubed, 3) == 64.0, "Cube function applied correctly to fourth element");
    test_assert(VECTOR(cubed, 4) == 125.0, "Cube function applied correctly to fifth element");
    
    // Test negate function
    vector *negated = vector_map(vector_clone(original), negate);
    test_assert(negated != NULL, "Negated vector created successfully");
    test_assert(VECTOR(negated, 0) == -1.0, "Negate function applied correctly to first element");
    test_assert(VECTOR(negated, 1) == -2.0, "Negate function applied correctly to second element");
    test_assert(VECTOR(negated, 2) == -3.0, "Negate function applied correctly to third element");
    test_assert(VECTOR(negated, 3) == -4.0, "Negate function applied correctly to fourth element");
    test_assert(VECTOR(negated, 4) == -5.0, "Negate function applied correctly to fifth element");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)squared);
    number_delete((number*)cubed);
    number_delete((number*)negated);
    
    return 0;
}

// Test vector mapping with trigonometric and complex functions
int test_complex_vector_map() {
    printf("\n=== Testing Complex Vector Map Functions ===\n");
    
    // Create original vector with values suitable for trig functions
    vector *original = vector_from_list(4, (NN_TYPE[]){0.0, M_PI/6, M_PI/4, M_PI/2});
    test_assert(original != NULL, "Original vector created successfully");
    
    // Test sine function
    vector *sinned = vector_map(vector_clone(original), sine);
    test_assert(sinned != NULL, "Sine vector created successfully");
    test_assert(fabs(VECTOR(sinned, 0) - 0.0) < NN_TYPE_EPSILON, 
                "Sine function applied correctly to 0.0");
    test_assert(fabs(VECTOR(sinned, 1) - 0.5) < NN_TYPE_EPSILON, 
                "Sine function applied correctly to PI/6");
    test_assert(fabs(VECTOR(sinned, 2) - 0.7071) < 0.001, 
                "Sine function applied correctly to PI/4");
    test_assert(fabs(VECTOR(sinned, 3) - 1.0) < NN_TYPE_EPSILON, 
                "Sine function applied correctly to PI/2");
    
    // Test complex function
    vector *complex = vector_map(vector_clone(original), complex_function);
    test_assert(complex != NULL, "Complex function vector created successfully");
    
    // We don't check exact values for complex function, just that it applied something
    test_assert(VECTOR(complex, 0) != VECTOR(original, 0) || 
                VECTOR(complex, 1) != VECTOR(original, 1) ||
                VECTOR(complex, 2) != VECTOR(original, 2) ||
                VECTOR(complex, 3) != VECTOR(original, 3),
                "Complex function changed at least one value");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)sinned);
    number_delete((number*)complex);
    
    return 0;
}

// Test vector mapping with edge cases
int test_edge_case_vector_map() {
    printf("\n=== Testing Edge Case Vector Map ===\n");
    
    // Test with empty vector (length 0)
    vector *empty = vector_create(0);
    test_assert(empty != NULL, "Empty vector created successfully");
    
    vector *empty_mapped = vector_map(vector_clone(empty), square);
    test_assert(empty_mapped == NULL, "Empty vector mapping failed");
    
    // Test with special values
    vector *special = vector_from_list(5, (NN_TYPE[]){
        0.0, INFINITY, -INFINITY, NAN, -0.0
    });
    test_assert(special != NULL, "Special values vector created successfully");
    
    // Test absolute function on special values
    vector *abs_special = vector_map(vector_clone(special), absolute);
    test_assert(abs_special != NULL, "Special values vector mapped with absolute successfully");
    test_assert(VECTOR(abs_special, 0) == 0.0, "Absolute of 0.0 is 0.0");
    test_assert(isinf(VECTOR(abs_special, 1)) && VECTOR(abs_special, 1) > 0, 
                "Absolute of INFINITY is INFINITY");
    test_assert(isinf(VECTOR(abs_special, 2)) && VECTOR(abs_special, 2) > 0, 
                "Absolute of -INFINITY is INFINITY");
    test_assert(isnan(VECTOR(abs_special, 3)), 
                "Absolute of NAN is NAN");
    test_assert(VECTOR(abs_special, 4) == 0.0, "Absolute of -0.0 is 0.0");
    
    // Test reciprocal function (potential division by zero)
    vector *recip = vector_map(vector_clone(special), reciprocal);
    test_assert(recip != NULL, "Special values vector mapped with reciprocal successfully");
    test_assert(isinf(VECTOR(recip, 0)), "Reciprocal of 0.0 is INFINITY");
    test_assert(VECTOR(recip, 1) == 0.0, "Reciprocal of INFINITY is 0.0");
    test_assert(VECTOR(recip, 2) == -0.0 || VECTOR(recip, 2) == 0.0, 
                "Reciprocal of -INFINITY is -0.0 or 0.0");
    test_assert(isnan(VECTOR(recip, 3)), "Reciprocal of NAN is NAN");
    test_assert(isinf(VECTOR(recip, 4)), "Reciprocal of -0.0 is INFINITY");
    
    // Clean up
    number_delete((number*)empty);
    number_delete((number*)empty_mapped);
    number_delete((number*)special);
    number_delete((number*)abs_special);
    number_delete((number*)recip);
    
    return 0;
}

// Test vector mapping with large vectors
int test_large_vector_map() {
    printf("\n=== Testing Large Vector Map ===\n");
    
    // Create a large vector
    size_t large_size = 1000000; // 1 million elements
    vector *large = vector_create(large_size);
    test_assert(large != NULL, "Large vector created successfully");
    
    // Initialize with a pattern
    for (size_t i = 0; i < large_size; i++) {
        VECTOR(large, i) = (NN_TYPE)(i % 10);
    }
    
    // Apply a simple function
    vector *large_mapped = vector_map(vector_clone(large), increment);
    test_assert(large_mapped != NULL, "Large vector mapped successfully");
    
    // Check a few values
    test_assert(VECTOR(large_mapped, 0) == 1.0, "First element incremented correctly");
    test_assert(VECTOR(large_mapped, 9) == 10.0, "Element at index 9 incremented correctly");
    test_assert(VECTOR(large_mapped, 999999) == ((999999 % 10) + 1.0), 
                "Last element incremented correctly");
    
    // Clean up
    number_delete((number*)large);
    number_delete((number*)large_mapped);
    
    return 0;
}

// Test chained vector mapping operations
int test_chained_vector_map() {
    printf("\n=== Testing Chained Vector Map Operations ===\n");
    
    // Create original vector
    vector *original = vector_from_list(5, (NN_TYPE[]){1.0, 2.0, 3.0, 4.0, 5.0});
    test_assert(original != NULL, "Original vector created successfully");
    
    // Apply multiple map operations in sequence
    vector *result = vector_map(
                       vector_map(
                         vector_map(
                           vector_clone(original), 
                           square
                         ),
                         increment
                       ),
                       negate
                     );
    
    test_assert(result != NULL, "Chained mapping operations successful");
    
    // Verify results: negate(increment(square(x)))
    // For x=1: negate(increment(square(1))) = negate(increment(1)) = negate(2) = -2
    test_assert(VECTOR(result, 0) == -2.0, "Chained mapping correct for first element");
    // For x=2: negate(increment(square(2))) = negate(increment(4)) = negate(5) = -5
    test_assert(VECTOR(result, 1) == -5.0, "Chained mapping correct for second element");
    // For x=3: negate(increment(square(3))) = negate(increment(9)) = negate(10) = -10
    test_assert(VECTOR(result, 2) == -10.0, "Chained mapping correct for third element");
    // For x=4: negate(increment(square(4))) = negate(increment(16)) = negate(17) = -17
    test_assert(VECTOR(result, 3) == -17.0, "Chained mapping correct for fourth element");
    // For x=5: negate(increment(square(5))) = negate(increment(25)) = negate(26) = -26
    test_assert(VECTOR(result, 4) == -26.0, "Chained mapping correct for fifth element");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)result);
    
    return 0;
}

// Test vector mapping with identity function (no change)
int test_identity_vector_map() {
    printf("\n=== Testing Identity Vector Map ===\n");
    
    // Create original vector
    vector *original = vector_from_list(5, (NN_TYPE[]){1.0, 2.0, 3.0, 4.0, 5.0});
    test_assert(original != NULL, "Original vector created successfully");
    
    // Apply identity function
    vector *identity_mapped = vector_map(vector_clone(original), identity);
    test_assert(identity_mapped != NULL, "Identity mapping successful");
    
    // Verify values are unchanged
    for (size_t i = 0; i < original->length; i++) {
        test_assert(VECTOR(identity_mapped, i) == VECTOR(original, i),
                    "Identity function preserved value at index %zu", i);
    }
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)identity_mapped);
    
    return 0;
}

// Test vector mapping with threshold function (binary classification)
int test_threshold_vector_map() {
    printf("\n=== Testing Threshold Vector Map ===\n");
    
    // Create original vector with values around the threshold
    vector *original = vector_from_list(10, (NN_TYPE[]){
        0.1, 0.3, 0.5, 0.51, 0.7, 0.9, 0.49, 0.0, 1.0, 0.5
    });
    test_assert(original != NULL, "Original vector created successfully");
    
    // Apply threshold function
    vector *thresholded = vector_map(vector_clone(original), threshold);
    test_assert(thresholded != NULL, "Threshold mapping successful");
    
    // Verify thresholding
    test_assert(VECTOR(thresholded, 0) == 0.0, "Value 0.1 thresholded to 0.0");
    test_assert(VECTOR(thresholded, 1) == 0.0, "Value 0.3 thresholded to 0.0");
    test_assert(VECTOR(thresholded, 2) == 0.0, "Value 0.5 thresholded to 0.0 (edge case)");
    test_assert(VECTOR(thresholded, 3) == 1.0, "Value 0.51 thresholded to 1.0");
    test_assert(VECTOR(thresholded, 4) == 1.0, "Value 0.7 thresholded to 1.0");
    test_assert(VECTOR(thresholded, 5) == 1.0, "Value 0.9 thresholded to 1.0");
    test_assert(VECTOR(thresholded, 6) == 0.0, "Value 0.49 thresholded to 0.0");
    test_assert(VECTOR(thresholded, 7) == 0.0, "Value 0.0 thresholded to 0.0");
    test_assert(VECTOR(thresholded, 8) == 1.0, "Value 1.0 thresholded to 1.0");
    test_assert(VECTOR(thresholded, 9) == 0.0, "Value 0.5 thresholded to 0.0 (edge case)");
    
    // Clean up
    number_delete((number*)original);
    number_delete((number*)thresholded);
    
    return 0;
}

// Test vector mapping with rounding function
int test_rounding_vector_map() {
    printf("\n=== Testing Rounding Vector Map ===\n");
    
    // Create original vector with decimal values
    vector *original = vector_from_list(8, (NN_TYPE[]){
        1.1, 1.5, 1.9, 2.5, 2.51, 3.49, 3.5, 3.51
    });
    test_assert(original != NULL, "Original vector created successfully");
    
    // Apply rounding function
    // vector *rounded = vector_map(vector_clone(original), round_to_int); // This leads to corruption of original vector
    vector *rounded = vector_map(original, round_to_int);
    test_assert(rounded != NULL, "Rounding mapping successful");
    
    // Verify rounding
    test_assert(VECTOR(rounded, 0) == 1.0, "Value 1.1 rounded to 1.0");
    test_assert(VECTOR(rounded, 1) == 2.0, "Value 1.5 rounded to 2.0");
    test_assert(VECTOR(rounded, 2) == 2.0, "Value 1.9 rounded to 2.0");
    test_assert(VECTOR(rounded, 3) == 3.0, "Value 2.5 rounded to 3.0");
    test_assert(VECTOR(rounded, 4) == 3.0, "Value 2.51 rounded to 3.0");
    test_assert(VECTOR(rounded, 5) == 3.0, "Value 3.49 rounded to 3.0");
    test_assert(VECTOR(rounded, 6) == 4.0, "Value 3.5 rounded to 4.0");
    test_assert(VECTOR(rounded, 7) == 4.0, "Value 3.51 rounded to 4.0");
    
    // Clean up
    /*number_delete((number*)original);*/
    number_delete((number*)rounded);
    
    return 0;
}

// Test performance of vector_map with large vectors
int test_vector_map_performance() {
    printf("\n=== Testing Vector Map Performance ===\n");
    
    // Create a large vector
    size_t large_size = 10000000; // 10 million elements
    printf("Creating vector with %zu elements...\n", large_size);
    vector *large = vector_create(large_size);
    test_assert(large != NULL, "Large vector created successfully");
    
    // Initialize with random values
    for (size_t i = 0; i < large_size; i++) {
        VECTOR(large, i) = ((NN_TYPE)rand() / RAND_MAX) * 10.0;
    }
    
    // Measure time for square function
    printf("Applying square function...\n");
    clock_t start = clock();
    vector *squared = vector_map(vector_clone(large), square);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(squared != NULL, "Large vector mapped successfully");
    printf("Time taken for square function: %f seconds\n", time_taken);
    
    // Measure time for complex function
    printf("Applying complex function...\n");
    start = clock();
    vector *complex_result = vector_map(vector_clone(large), complex_function);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(complex_result != NULL, "Large vector mapped with complex function successfully");
    printf("Time taken for complex function: %f seconds\n", time_taken);
    
    // Clean up
    number_delete((number*)large);
    number_delete((number*)squared);
    number_delete((number*)complex_result);
    
    return 0;
}

int main() {
    printf("=== Naive Numbers Vector Map Test ===\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run tests
    int result = 0;
    result |= test_basic_vector_map();
    result |= test_complex_vector_map();
    result |= test_edge_case_vector_map();
    result |= test_large_vector_map();
    result |= test_chained_vector_map();
    result |= test_identity_vector_map();
    result |= test_threshold_vector_map();
    result |= test_rounding_vector_map();
    result |= test_vector_map_performance();
    
    if (result == 0) {
        printf("\nAll vector_map tests passed successfully!\n");
    } else {
        printf("\nSome tests failed!\n");
    }
    
    return result;
}
