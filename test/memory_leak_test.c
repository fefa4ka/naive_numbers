 /**
  * Memory leak test for the Naive Numbers library
  *
  * This test checks for memory leaks when using reference counting
  * and chained operations.
  */

 #include <nn.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>

 // Custom function for vector_map
 NN_TYPE square(NN_TYPE x) {
     return x * x;
 }

 // Test basic reference counting
 void test_basic_ref_counting() {
     printf("\n=== Testing Basic Reference Counting ===\n");

     // Create a number and increment its reference count
     number *n = number_create(3.14);
     printf("Initial ref count: %zu\n", n->ref_count);

     number_ref(n);
     printf("After number_ref: %zu\n", n->ref_count);

     number_ref(n);
     printf("After another number_ref: %zu\n", n->ref_count);

     // Decrement reference count
     number_unref(n);
     printf("After number_unref: %zu\n", n->ref_count);

     number_unref(n);
     printf("After another number_unref: %zu\n", n->ref_count);

     // Final unref should free the memory
     number_unref(n);
     printf("Final number_unref called (memory should be freed)\n");
 }

 // Test vector operations with reference counting
 void test_vector_operations() {
     printf("\n=== Testing Vector Operations with Reference Counting ===\n");

     // Create vectors
     vector *v1 = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
     vector *v2 = vector_from_list(3, (NN_TYPE[]){4.0, 5.0, 6.0});

     printf("v1 initial ref count: %zu\n", v1->number.ref_count);
     printf("v2 initial ref count: %zu\n", v2->number.ref_count);

     // Perform operations that should handle reference counting correctly
     vector *result = vector_addition(
                        vector_multiplication(
                          vector_map(vector_clone(v1), square),
                          float_create(2.0)
                        ),
                        (number *)v2
                      );

     printf("v1 ref count after operations: %zu\n", v1->number.ref_count);
     printf("result ref count: %zu\n", result->number.ref_count);

     // Clean up
     number_delete(v1);
     number_delete(result);
 }

 // Test matrix operations with reference counting
 void test_matrix_operations() {
     printf("\n=== Testing Matrix Operations with Reference Counting ===\n");

     // Create matrices
     matrix *A = matrix_create_from_list(2, 2, (NN_TYPE[]){1.0, 2.0, 3.0, 4.0});
     matrix *B = matrix_identity(2, 2.0);

     printf("A initial ref count: %zu\n", A->number.ref_count);
     printf("B initial ref count: %zu\n", B->number.ref_count);

     // Perform operations that should handle reference counting correctly
     matrix *result = matrix_multiplication(
                        matrix_transpose(matrix_clone(A)),
                        B
                      );

     printf("A ref count after operations: %zu\n", A->number.ref_count);
     printf("result ref count: %zu\n", result->number.ref_count);

     // Clean up
     number_delete(A);
     number_delete(result);
 }

 // Test complex chained operations
 void test_complex_chained_operations() {
     printf("\n=== Testing Complex Chained Operations ===\n");

     // Create initial vectors
     vector *v1 = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});
     vector *v2 = vector_from_list(3, (NN_TYPE[]){0.5, 1.5, 2.5});

     printf("v1 initial ref count: %zu\n", v1->number.ref_count);
     printf("v2 initial ref count: %zu\n", v2->number.ref_count);

     // This creates a deeply nested chain of operations
     vector *v1_clone = vector_clone(v1);
     vector *v2_clone = vector_clone(v2);
     
     vector *result = vector_addition(
                        vector_multiplication(
                          vector_addition(
                            vector_map(v1_clone, square),
                            vector_map(v2_clone, square)
                          ),
                          float_create(0.5)
                        ),
                        (number *)number_ref((number*)v1)  // Explicitly ref v1 since we're using it directly
                      );

     printf("v1 ref count after operations: %zu\n", v1->number.ref_count);
     printf("v2 ref count after operations: %zu\n", v2->number.ref_count);
     printf("result ref count: %zu\n", result->number.ref_count);

     // Clean up
     number_delete((number*)v1);
     number_delete((number*)v2);
     number_delete((number*)result);
 }

 // Test repeated operations
 void test_repeated_operations() {
     printf("\n=== Testing Repeated Operations ===\n");

     // Create a vector
     vector *v = vector_from_list(3, (NN_TYPE[]){1.0, 2.0, 3.0});

     printf("Initial ref count: %zu\n", v->number.ref_count);

     // Perform the same operation multiple times
     for (int i = 0; i < 1000; i++) {
         vector_addition(v, float_create(0.001));
     }

     printf("Ref count after 1000 operations: %zu\n", v->number.ref_count);

     // Clean up
     number_delete(v);
 }

 int main() {
     printf("=== Naive Numbers Memory Leak Test ===\n");

     // Seed random number generator
     srand(time(NULL));

     // Run tests
     test_basic_ref_counting();
     test_vector_operations();
     test_matrix_operations();
     test_complex_chained_operations();
     test_repeated_operations();

     printf("\nAll memory leak tests completed.\n");
     printf("Run with valgrind to check for memory leaks:\n");
     printf("valgrind --leak-check=full ./memory_leak_test\n");

     return 0;
 }
