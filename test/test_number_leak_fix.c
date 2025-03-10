#include <nn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Test helper function
void test_assert(int condition, const char* message) {
    if (condition) {
        printf("✓ %s\n", message);
    } else {
        printf("✗ %s\n", message);
    }
}

int test_number_creation() {
    printf("\n=== Testing Number Creation ===\n");
    
    // Create different types of numbers
    number *n = number_create(3.14);
    number *i = integer_create(42);
    number *f = float_create(2.718);
    number *d = double_create(1.618);
    
    // Verify types
    test_assert(n->type == NN_FLOAT, "Float type is correct");
    test_assert(i->type == NN_INTEGER, "Integer type is correct");
    test_assert(f->type == NN_FLOAT, "Float type is correct");
    test_assert(d->type == NN_DOUBLE, "Double type is correct");
    
    // Clean up
    number_delete(n);
    number_delete(i);
    number_delete(f);
    number_delete(d);
    
    return 0;
}

int test_vector_reference_counting() {
    printf("\n=== Testing Vector Reference Counting ===\n");
    
    // Create vectors
    vector *v1 = vector_create(3);
    vector_seed(v1, 1.0);
    vector *v2 = vector_create(3);
    vector_seed(v2, 2.0);
    
    // Test initial reference counts
    test_assert(v1->number.ref_count == 1, "Initial reference count is 1");
    test_assert(v2->number.ref_count == 1, "Initial reference count is 1");
    
    // Test reference counting
    vector *v1_ref = (vector*)number_ref((number*)v1);
    test_assert(v1->number.ref_count == 2, "Reference count incremented to 2");
    
    // Clean up
    number_unref((number*)v1_ref);
    test_assert(v1->number.ref_count == 1, "Reference count decremented to 1");
    
    // Clean up vectors
    number_delete(v1);
    number_delete(v2);
    
    return 0;
}

int test_null_reference_counting() {
    printf("\n=== Testing NULL Reference Counting ===\n");
    
    // Test number_ref with NULL
    number *n_null = NULL;
    number *n_ref = number_ref(n_null);
    test_assert(n_ref == NULL, "number_ref handles NULL correctly");
    
    // Test number_unref with NULL
    number_unref(n_null);  // Should not crash
    
    return 0;
}

int test_complex_memory_management() {
    printf("\n=== Testing Complex Memory Management ===\n");
    
    // Create a matrix
    matrix *m = matrix_create(3, 3);
    matrix_seed(m, 1.0);
    
    // Create a vector
    vector *v = vector_create(3);
    vector_seed(v, 2.0);
    
    // Perform operations
    vector *result = vector_transformation_by_matrix(m, v);
    
    // Clean up
    number_delete(m);
    number_delete(result);
    
    return 0;
}

int main() {
    printf("=== Naive Numbers Number Test ===\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run tests
    int result = 0;
    result |= test_number_creation();
    result |= test_vector_reference_counting();
    result |= test_null_reference_counting();
    result |= test_complex_memory_management();
    
    return result;
}
