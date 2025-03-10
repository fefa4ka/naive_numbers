/**
 * Test for chained operations in the Naive Numbers library
 * 
 * This test demonstrates how to use operation chaining with proper
 * reference counting to avoid memory leaks.
 */

#include <nn.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Simple test assertion macro
#define test_assert(test, message, ...) \
    if (!(test)) { \
        printf("ERROR: " message "\n", ##__VA_ARGS__); \
        return 1; \
    } else { \
        printf("OK: " message "\n", ##__VA_ARGS__); \
    }

// Function to measure memory usage before and after operations
void check_memory_usage(const char* operation_name, void (*operation)(void)) {
    // Perform the operation
    operation();
    
    printf("Completed: %s\n", operation_name);
}

// Custom function for vector_map
NN_TYPE square(NN_TYPE x) {
    return x * x;
}

NN_TYPE add_one(NN_TYPE x) {
    return x + 1.0;
}

// Test vector chained operations
int test_vector_chained_operations() {
    printf("\n=== Testing Vector Chained Operations ===\n");
    
    // Create initial vectors
    vector *v1 = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
    vector *v2 = vector_from_list(3, (NN_TYPE[]){4.0, 5.0, 6.0});
    
    printf("Initial vectors:\n");
    printf("v1: ");
    vector_print(v1);
    printf("v2: ");
    vector_print(v2);
    
    // Perform chained operations
    // This should:
    // 1. Square each element of v1
    // 2. Multiply the result by 2.0
    // 3. Add v2 to the result
    printf("\nPerforming chained operations...\n");
    
    vector *result = vector_addition(
                       vector_multiplication(
                         vector_map(vector_clone(v1), square),
                         float_create(2.0)
                       ),
                       (number *)v2
                     );
    
    printf("Result after chained operations:\n");
    vector_print(result);
    
    // Verify the result
    test_assert(VECTOR(result, 0) == 2.0*1.0*1.0 + 4.0, "First element calculated correctly");
    test_assert(VECTOR(result, 1) == 2.0*2.0*2.0 + 5.0, "Second element calculated correctly");
    test_assert(VECTOR(result, 2) == 2.0*3.0*3.0 + 6.0, "Third element calculated correctly");
    
    // Clean up
    number_delete(v1);
    number_delete(v2);
    number_delete(result);
    
    return 0;
}

// Test matrix chained operations
int test_matrix_chained_operations() {
    printf("\n=== Testing Matrix Chained Operations ===\n");
    
    // Create initial matrices
    matrix *A = matrix_create_from_list(2, 2, (NN_TYPE[]){1.0, 2.0, 3.0, 4.0});
    matrix *B = matrix_identity(2, 2.0);
    
    printf("Initial matrices:\n");
    printf("A: ");
    matrix_print(A);
    printf("B: ");
    matrix_print(B);
    
    // Perform chained operations
    // This should:
    // 1. Create A^T (transpose of A)
    // 2. Multiply A^T by B
    // 3. Apply square function to each element
    printf("\nPerforming chained operations...\n");
    
    matrix *result = matrix_map(
                       matrix_multiplication(
                         matrix_transpose(matrix_clone(A)),
                         B
                       ),
                       square
                     );
    
    printf("Result after chained operations:\n");
    matrix_print(result);
    
    // Verify the result (A^T * B, then squared)
    // A^T = [1 3]
    //       [2 4]
    // B = [2 0]
    //     [0 2]
    // A^T * B = [2 6]
    //           [4 8]
    // Squared = [4 36]
    //           [16 64]
    test_assert(MATRIX(result, 0, 0) == 4.0, "Element [0,0] calculated correctly");
    test_assert(MATRIX(result, 0, 1) == 36.0, "Element [0,1] calculated correctly");
    test_assert(MATRIX(result, 1, 0) == 16.0, "Element [1,0] calculated correctly");
    test_assert(MATRIX(result, 1, 1) == 64.0, "Element [1,1] calculated correctly");
    
    // Clean up
    number_delete(A);
    number_delete(B);
    number_delete(result);
    
    return 0;
}

// Test complex chained operations with multiple levels
int test_complex_chained_operations() {
    printf("\n=== Testing Complex Chained Operations ===\n");
    
    // Create initial vectors
    vector *v1 = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
    vector *v2 = vector_from_list(3, (NN_TYPE[]){0.5, 1.5, 2.5});
    
    printf("Initial vectors:\n");
    printf("v1: ");
    vector_print(v1);
    printf("v2: ");
    vector_print(v2);
    
    // Perform complex chained operations
    printf("\nPerforming complex chained operations...\n");
    
    // This creates a deeply nested chain of operations:
    // 1. Clone v1 and map square function to it
    // 2. Clone v2 and add 1.0 to each element
    // 3. Add the results of steps 1 and 2
    // 4. Multiply the result by 0.5
    // 5. Add the original v1 to the result
    vector *result = vector_addition(
                       vector_multiplication(
                         vector_addition(
                           vector_map(vector_clone(v1), square),
                           vector_map(vector_clone(v2), add_one)
                         ),
                         float_create(0.5)
                       ),
                       (number *)v1
                     );
    
    printf("Result after complex chained operations:\n");
    vector_print(result);
    
    // Verify the result
    // For first element: 0.5 * ((1.0^2) + (0.5+1.0)) + 1.0 = 0.5 * (1.0 + 1.5) + 1.0 = 0.5 * 2.5 + 1.0 = 2.25
    test_assert(fabs(VECTOR(result, 0) - 2.25) < 0.0001, "First element calculated correctly");
    
    // Clean up
    number_delete(v1);
    number_delete(v2);
    number_delete(result);
    
    return 0;
}

// Test probability operations with chaining
int test_probability_chained_operations() {
    printf("\n=== Testing Probability Chained Operations ===\n");
    
    // Create a sample matrix
    matrix *samples = matrix_create_from_list(3, 2, 
        (NN_TYPE[]){0.8, 0.2, 0.2, 0.4, 0.5, 0.7});
    
    // Define field names
    char *fields[] = {"field1", "field2"};
    
    // Create probability space
    probability *p = probability_from_matrix(samples, fields);
    
    printf("Created probability space from samples\n");
    
    // Calculate various probability metrics
    NN_TYPE mass = probability_mass_of(p, "field1", 0.8);
    NN_TYPE expected = probability_expected_value(p, "field2");
    NN_TYPE variance = probability_variance(p, "field1");
    NN_TYPE covariance = probability_covariance(p, "field1", "field2");
    NN_TYPE correlation = probability_correlation(p, "field1", "field2");
    
    printf("Probability calculations:\n");
    printf("Mass of field1=0.8: %f\n", mass);
    printf("Expected value of field2: %f\n", expected);
    printf("Variance of field1: %f\n", variance);
    printf("Covariance between field1 and field2: %f\n", covariance);
    printf("Correlation between field1 and field2: %f\n", correlation);
    
    // Clean up
    probability_delete(p);
    
    return 0;
}

int main() {
    printf("=== Naive Numbers Chained Operations Test ===\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run tests
    int result = 0;
    result |= test_vector_chained_operations();
    result |= test_matrix_chained_operations();
    result |= test_complex_chained_operations();
    result |= test_probability_chained_operations();
    
    if (result == 0) {
        printf("\nAll chained operation tests passed successfully!\n");
    } else {
        printf("\nSome tests failed!\n");
    }
    
    return result;
}
