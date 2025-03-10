/**
 * Test for vector creation and manipulation in the Naive Numbers library
 *
 * This test verifies the basic functionality of vector operations,
 * including creation, deletion, and various manipulation functions.
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

// Test vector creation methods
int test_vector_creation() {
    printf("\n=== Testing Vector Creation ===\n");
    
    // Test vector_create
    vector *v1 = vector_create(5);
    test_assert(v1 != NULL, "vector_create allocates memory");
    test_assert(v1->length == 5, "vector_create sets correct length");
    test_assert(v1->number.type == NN_VECTOR, "vector_create sets correct type");
    test_assert(v1->number.ref_count == 1, "vector_create sets initial ref_count to 1");
    
    // Test vector_seed
    vector_seed(v1, 3.14);
    test_assert((VECTOR(v1, 0) - 3.14) < NN_TYPE_EPSILON, "vector_seed sets values correctly, inside = %f", VECTOR(v1, 0));
    test_assert((VECTOR(v1, 4) - 3.14) < NN_TYPE_EPSILON, "vector_seed sets all values");
    
    // Test vector_from_list
    NN_TYPE values[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    vector *v2 = vector_from_list(5, values);
    test_assert(v2 != NULL, "vector_from_list allocates memory");
    test_assert(v2->length == 5, "vector_from_list sets correct length");
    test_assert((VECTOR(v2, 0) - 1.1)  < NN_TYPE_EPSILON, "vector_from_list sets first value correctly");
    test_assert((VECTOR(v2, 4) - 5.5)  < NN_TYPE_EPSILON, "vector_from_list sets last value correctly");
    
    // Test vector_clone
    vector *v3 = vector_clone(v2);
    test_assert(v3 != NULL, "vector_clone allocates memory");
    test_assert(v3->length == v2->length, "vector_clone preserves length");
    test_assert(VECTOR(v3, 0) == VECTOR(v2, 0), "vector_clone preserves first value");
    test_assert(VECTOR(v3, 4) == VECTOR(v2, 4), "vector_clone preserves last value");
    test_assert(v3 != v2, "vector_clone creates a new instance");
    test_assert(v3->number.values != v2->number.values, "vector_clone creates new values array");
    
    // Test vector_reshape
    vector *v4 = vector_reshape(vector_clone(v2), 8);
    test_assert(v4 != NULL, "vector_reshape returns non-NULL");
    test_assert(v4->length == 8, "vector_reshape changes length correctly");
    test_assert((VECTOR(v4, 0) - 1.1) < NN_TYPE_EPSILON, "vector_reshape preserves existing values");
    test_assert((VECTOR(v4, 4) - 5.5) < NN_TYPE_EPSILON, "vector_reshape preserves last original value");
    test_assert((VECTOR(v4, 5) - 0.0) < NN_TYPE_EPSILON, "vector_reshape initializes new values to 0");
    
    // Clean up
    number_delete((number*)v1);
    number_delete((number*)v2);
    number_delete((number*)v3);
    number_delete((number*)v4);
    
    return 0;
}

// Test vector manipulation operations
int test_vector_manipulation() {
    printf("\n=== Testing Vector Manipulation ===\n");
    
    // Create test vectors
    vector *v1 = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
    vector *v2 = vector_from_list(3, (NN_TYPE[]){4.0, 5.0, 6.0});
    
    // Test vector_addition
    vector *result = vector_addition(vector_clone(v1), (number*)v2);
    test_assert(result != NULL, "vector_addition returns non-NULL");
    test_assert(VECTOR(result, 0) == 5.0, "vector_addition correctly adds first elements");
    test_assert(VECTOR(result, 2) == 9.0, "vector_addition correctly adds last elements");
    
    // Test vector_subtraction
    vector *v3 = vector_clone(v1);
    vector *diff = vector_subtraction(v3, (number*)number_ref((number*)v2));
    test_assert(diff != NULL, "vector_subtraction returns non-NULL");
    test_assert((VECTOR(diff, 0) + 3.0) < NN_TYPE_EPSILON, "vector_subtraction correctly subtracts first elements, %f - %f = %f", VECTOR(v3, 0),VECTOR(v2,0),VECTOR(diff,0));
    test_assert((VECTOR(diff, 2) + 3.0) < NN_TYPE_EPSILON, "vector_subtraction correctly subtracts last elements");
    
    // Test vector_multiplication with scalar
    vector *v4 = vector_clone(v1);
    vector *scaled = vector_multiplication(v4, number_create(2.0));
    test_assert(scaled != NULL, "vector_multiplication returns non-NULL");
    test_assert(VECTOR(scaled, 0) == 2.0, "vector_multiplication correctly scales first element");
    test_assert(VECTOR(scaled, 2) == 6.0, "vector_multiplication correctly scales last element");
    
    // Test vector_dot_product
    NN_TYPE dot = vector_dot_product(v1, (vector*)number_ref((number*)v2));
    test_assert(fabs(dot - 32.0) < 0.0001, "vector_dot_product calculates correct result");
    
    // Test vector_length
    NN_TYPE length = vector_length(v1);
    test_assert(fabs(length - sqrt(14.0)) < 0.0001, "vector_length calculates correct result");
    
    // Test vector_unit
    vector *unit = vector_unit(v1);
    test_assert(unit != NULL, "vector_unit returns non-NULL");
    test_assert(fabs(vector_length(unit) - 1.0) < 0.0001, "vector_unit creates vector of length 1");
    
    // Clean up
    number_delete((number*)v1);
    number_delete((number*)v2);
    number_delete((number*)result);
    number_delete((number*)diff);
    number_delete((number*)scaled);
    number_delete((number*)unit);
    
    return 0;
}

NN_TYPE square(NN_TYPE x) { return x * x; }
NN_TYPE add_one(NN_TYPE x) { return x + 1.0; }
// Test vector map operations
int test_vector_map() {
    printf("\n=== Testing Vector Map Operations ===\n");
    
    // Create test vector
    vector *v = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
    
    // Define map functions
    
    // Test vector_map with square function
    vector *v_squared = vector_map(vector_clone(v), square);
    test_assert(v_squared != NULL, "vector_map returns non-NULL");
    test_assert(VECTOR(v_squared, 0) == 1.0, "vector_map correctly applies square to first element");
    test_assert(VECTOR(v_squared, 1) == 4.0, "vector_map correctly applies square to middle element");
    test_assert(VECTOR(v_squared, 2) == 9.0, "vector_map correctly applies square to last element");
    
    // Test vector_map with add_one function
    vector *v_plus_one = vector_map(vector_clone(v), add_one);
    test_assert(v_plus_one != NULL, "vector_map returns non-NULL");
    test_assert(VECTOR(v_plus_one, 0) == 2.0, "vector_map correctly applies add_one to first element");
    test_assert(VECTOR(v_plus_one, 1) == 3.0, "vector_map correctly applies add_one to middle element");
    test_assert(VECTOR(v_plus_one, 2) == 4.0, "vector_map correctly applies add_one to last element");
    
    // Clean up
    number_delete((number*)v);
    number_delete((number*)v_squared);
    number_delete((number*)v_plus_one);
    
    return 0;
}

// Test vector utility functions
int test_vector_utilities() {
    printf("\n=== Testing Vector Utility Functions ===\n");
    
    // Create test vectors
    vector *v1 = vector_from_list(5, (NN_TYPE[]){1.0, 2.0, 3.0, 2.0, 1.0});
    vector *v2 = vector_from_list(3, (NN_TYPE[]){0.0, 1.0, 0.0});
    
    // Test vector_sum
    NN_TYPE sum = vector_sum(v1);
    test_assert(fabs(sum - 9.0) < 0.0001, "vector_sum calculates correct result");
    
    // Test vector_sum_to
    NN_TYPE sum_to = vector_sum_to(v1, 2);
    test_assert(fabs(sum_to - 6.0) < 0.0001, "vector_sum_to calculates correct result");
    
    // Test vector_sum_between
    NN_TYPE sum_between = vector_sum_between(v1, 1, 4);
    test_assert(fabs(sum_between - 7.0) < 0.0001, "vector_sum_between calculates correct result");
    
    // Test vector_unique
    vector *unique = vector_unique(v1);
    test_assert(unique != NULL, "vector_unique returns non-NULL");
    test_assert(unique->length == 3, "vector_unique finds correct number of unique values");
    
    // Test vector_index_of
    int index = vector_index_of(v1, 3.0);
    test_assert(index == 2, "vector_index_of finds correct index");
    
    int not_found = vector_index_of(v1, 99.0);
    test_assert(not_found == -1, "vector_index_of returns -1 for values not in vector");
    
    // Test vector_max_index
    size_t max_idx = vector_max_index(v1);
    test_assert(max_idx == 2, "vector_max_index finds index of maximum value");
    
    // Test vector_non_zero_length
    size_t non_zeros = vector_non_zero_length(v2);
    test_assert(non_zeros == 1, "vector_non_zero_length counts correct number of non-zero elements");
    
    // Clean up
    number_delete((number*)v1);
    number_delete((number*)v2);
    number_delete((number*)unique);
    
    return 0;
}

// Test vector angle calculations
int test_vector_angles() {
    printf("\n=== Testing Vector Angle Calculations ===\n");
    
    // Create test vectors
    vector *v1 = vector_from_list(2, (NN_TYPE[]){1.0, 0.0});  // Unit vector along x-axis
    vector *v2 = vector_from_list(2, (NN_TYPE[]){0.0, 1.0});  // Unit vector along y-axis
    vector *v3 = vector_from_list(2, (NN_TYPE[]){1.0, 1.0});  // 45-degree vector
    
    // Test vector_angle
    NN_TYPE angle1 = vector_angle(v1, (vector*)number_ref((number*)v2));
    test_assert(fabs(angle1 - 90.0) < 0.0001, "vector_angle calculates 90 degrees correctly");
    
    NN_TYPE angle2 = vector_angle(v1, (vector*)number_ref((number*)v3));
    test_assert(fabs(angle2 - 45.0) < 0.0001, "vector_angle calculates 45 degrees correctly");
    
    // Test vector_is_perpendicular
    int perp1 = vector_is_perpendicular(v1, (vector*)number_ref((number*)v2));
    test_assert(perp1 == 1, "vector_is_perpendicular correctly identifies perpendicular vectors");
    
    int perp2 = vector_is_perpendicular(v1, (vector*)number_ref((number*)v3));
    test_assert(perp2 == 0, "vector_is_perpendicular correctly identifies non-perpendicular vectors");
    
    // Clean up
    number_delete((number*)v1);
    number_delete((number*)v2);
    number_delete((number*)v3);
    
    return 0;
}

// Test memory management with vectors
int test_vector_memory_management() {
    printf("\n=== Testing Vector Memory Management ===\n");
    
    // Create test vectors
    vector *v1 = vector_create(3);
    vector_seed(v1, 1.0);
    
    // Test reference counting
    test_assert(v1->number.ref_count == 1, "Initial reference count is 1");
    
    vector *v1_ref = (vector*)number_ref((number*)v1);
    test_assert(v1->number.ref_count == 2, "Reference count incremented to 2");
    test_assert(v1 == v1_ref, "number_ref returns the same pointer");
    
    // Test that operations maintain proper reference counting
    vector *result = vector_addition(
                       vector_multiplication(
                         vector_clone(v1),
                         float_create(2.0)
                       ),
                       (number*)vector_create(3)
                     );
    
    test_assert(result != NULL, "Chained operations return non-NULL result");
    test_assert(v1->number.ref_count == 2, "Original vector reference count unchanged after operations");
    
    // Clean up
    number_unref((number*)v1_ref);
    test_assert(v1->number.ref_count == 1, "Reference count decremented to 1");
    
    number_delete((number*)v1);
    number_delete((number*)result);
    
    return 0;
}

int main() {
    printf("=== Naive Numbers Vector Creation and Manipulation Test ===\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run tests
    int result = 0;
    result |= test_vector_creation();
    result |= test_vector_manipulation();
    result |= test_vector_map();
    result |= test_vector_utilities();
    result |= test_vector_angles();
    result |= test_vector_memory_management();
    
    if (result == 0) {
        printf("\nAll vector tests passed successfully!\n");
    } else {
        printf("\nSome tests failed!\n");
    }
    
    return result;
}
