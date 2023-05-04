#include "vector.h"
#include <math.h>
#include <nn.h> // include header for Linked Ring library
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define log_print(type, message, ...)                                          \
    printf(type "\t" message "\n", ##__VA_ARGS__)
#define log_debug(type, message, ...)                                          \
    log_print(type, message " (%s:%d)\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define log_verbose(message, ...) log_print("VERBOSE", message, ##__VA_ARGS__)
#define log_info(message, ...)    log_print("INFO", message, ##__VA_ARGS__)
#define log_ok(message, ...)      log_print("OK", message, ##__VA_ARGS__)
#define log_error(message, ...)                                                \
    log_print("\e[1m\e[31mERROR\e[39m\e[0m", message " (%s:%d)\n",             \
              ##__VA_ARGS__, __FILE__, __LINE__)

#define profiler_tick()                                                        \
    {                                                                          \
        clock_t begin, end, passed;                                            \
        begin = clock()

#define profiler_tock()                                                        \
    end    = clock();                                                          \
    passed = end - begin;                                                      \
    printf("passed: %ld\n", passed);                                           \
    }
#define test_assert(test, message, ...)                                        \
    if (!(test)) {                                                             \
        log_error(message, ##__VA_ARGS__);                                     \
        return 1;                                                              \
    } else {                                                                   \
        log_ok(message, ##__VA_ARGS__);                                        \
    }


// Function to measure the time taken for a function to execute
clock_t measure_time(void (*func)(vector *, number *), vector *v, void *w)
{
    clock_t start, end, passed;

    const int num_trials = 1000;
    clock_t   total_time = 0;
    for (int i = 0; i < num_trials; i++) {
        start = clock();
        func(v, w);
        end    = clock();
        passed = end - start;
        total_time += passed;
    }
    return total_time / num_trials;
}

float sigmoid(float value) { return 1.f / (1.f + exp(value * -1.f)); }

int test_vector_create()
{
    int r;
    number *one_two_three;
    float   one_23 = 1.23;
    one_two_three  = float_create(1.23);
    test_assert(one_two_three, "Number allocated");
    test_assert(one_two_three->floated == one_23, "1.23 initialzed");

    r = number_delete(one_two_three);
    test_assert(r == 0, "Number deleted");

    return 0;
}

int test_vector_clone()
{
    int r;
    vector *v1 = vector_from_list(5, (NN_TYPE[]){3, 1, 3, 3, 7});
    vector *v2 = vector_clone(v1);

    test_assert(v1->length == v2->length, "Size euqal");
    for (int i = 0; i < v1->length; i++) {
        test_assert(VECTOR(v1, i) == VECTOR(v2, i), "Vector elements #%d equal",
                    i);
    }

    r = number_delete(v1);
    test_assert(r == 0, "Number deleted");
    r = number_delete(v2);
    test_assert(r == 0, "Number deleted");

    return 0;
}

int test_vector_length()
{
    vector *v1       = vector_from_list(3, (NN_TYPE[]){1, 2, 3});
    float   length   = vector_length(v1);
    float   expected = sqrt(1 + 4 + 9);

    test_assert(fabs(length - expected) < 0.0001, "Length as expected");

    number_delete(v1);

    return 0;
}

int test_vector_unit()
{
    // Test vector_unit with a non-zero vector
    vector *v1      = vector_from_list(2, (NN_TYPE[]){3.0, 4.0});
    vector *v1_unit = vector_unit(v1);
    test_assert(v1_unit, "Unit Vector Created");
    test_assert(fabs(VECTOR(v1_unit, 0) - 0.6) < 0.1, "Unit #0 value");
    test_assert(fabs(VECTOR(v1_unit, 1) - 0.8) < 0.1, "Unit #2 value");

    // Test vector_unit with a zero vector
//    vector *v2      = vector_from_list(2, (NN_TYPE[]){0.0, 0.0});
//    vector *v2_unit = vector_unit(v2);
    // FIXME: nan
    // vector_print(v2_unit);
    // test_assert(VECTOR(v2_unit, 0) == 0.0, "Zero vector unit %f",
    // VECTOR(v2_unit, 0)); test_assert(VECTOR(v2_unit, 1) == 0.0, "Zero vector
    // unit %f", VECTOR(v2_unit, 1));
    //number_delete(v2);
    //number_delete(v2_unit);

    number_delete(v1);
    number_delete(v1_unit);

    return 0;
}

int test_vector_2d() {
    int r;
     vector *vector_2d;
    float   vec_2d_data[] = {1, 2};

    vector_2d             = vector_create(2);
    test_assert(vector_2d, "2D Vector created");
    VECTOR(vector_2d, 0) = vec_2d_data[0];
    VECTOR(vector_2d, 1) = vec_2d_data[1];
    test_assert(*(float *)vector_2d->number.values == vec_2d_data[0],
                "2D vector data written %f", VECTOR(vector_2d, 0));
    test_assert(((float *)vector_2d->number.values)[1] == vec_2d_data[1],
                "2D vector data written %f", VECTOR(vector_2d, 1));

    r = number_delete(vector_2d);
    test_assert(r == 0, "2D Vector deleted");

    return 0;
}

int test_vector_3d() {
    int r;
    vector *vector_3d;
    vector *vector_3d_copy;
    float   vec_3d_data[] = {3, 4, 5};

    vector_3d = vector_from_list(3, vec_3d_data);
    test_assert(vector_3d, "3D Vector created");
    test_assert(VECTOR(vector_3d, 0) == vec_3d_data[0],
                "3D vector data written %f", VECTOR(vector_3d, 0));
    test_assert(VECTOR(vector_3d, 1) == vec_3d_data[1],
                "3D vector data written %f", VECTOR(vector_3d, 1));
    test_assert(VECTOR(vector_3d, 2) == vec_3d_data[2],
                "3D vector data written %f", VECTOR(vector_3d, 2));

    vector_3d_copy = vector_clone(vector_3d);
    test_assert(VECTOR(vector_3d_copy, 0) == vec_3d_data[0],
                "3D vector data written %f", VECTOR(vector_3d, 0));
    test_assert(VECTOR(vector_3d_copy, 1) == vec_3d_data[1],
                "3D vector data written %f", VECTOR(vector_3d, 1));
    test_assert(VECTOR(vector_3d_copy, 2) == vec_3d_data[2],
                "3D vector data written %f", VECTOR(vector_3d, 2));

    r = number_delete(vector_3d);
    test_assert(r == 0, "3D Vector deleted");

    r = number_delete(vector_3d_copy);
    test_assert(r == 0, "3D Vector Copy deleted");

    return 0;
}

int test_vector_reshape() {
    int     r;
    vector *vector_tmp;
    vector *vector_3d;

    vector_3d = vector_create(3);
    vector_3d = vector_seed(vector_3d, 0);
    vector_tmp = vector_reshape(vector_3d, 32);

    test_assert(vector_tmp, "Vector reshaped");
    test_assert(vector_3d == vector_tmp, "Vector reshaped");
    test_assert(vector_3d->length == 32, "Vector reshaped to length = %ld",
                vector_3d->length);

    r = number_delete(vector_3d);
    test_assert(r == 0, "3D Vector deleted");

    return 0;
}

int test_vector_addition() {
    int     r;
    vector *vector_tmp;
    vector *vector_3d;
    vector *big;
    vector *big_yet;
    number *one;
    number *pi;
    float   vec_3d_data[] = {3, 4, 5};

    pi = float_create(3.14f);
    vector_3d = vector_from_list(3, vec_3d_data);
    vector_tmp = vector_addition(vector_3d, pi);

    test_assert(VECTOR(vector_3d, 0) == (float)(vec_3d_data[0] + pi->floated),
                "Vector added %f + %f == %f", VECTOR(vector_3d, 0), pi->floated,
                (float)(vec_3d_data[0] + pi->floated));
    test_assert(VECTOR(vector_3d, 1) == (vec_3d_data[1] + pi->floated),
                "Vector added %f", VECTOR(vector_3d, 1));
    test_assert(VECTOR(vector_3d, 2) == (vec_3d_data[2] + pi->floated),
                "Vector added %f", VECTOR(vector_3d, 2));
    test_assert(VECTOR(vector_3d, 3) == pi->floated, "Vector added %f",
                VECTOR(vector_3d, 3));


    one     = float_create(2.0);
    big     = vector_create(122880);
    big_yet = vector_create(122880);
    vector_addition(big, one);
    vector_addition(big_yet, one);

    number_delete(one);
    number_delete(big);
    number_delete(big_yet);

    number_delete(pi);
    number_delete(vector_3d);

    return 0;
}

int test_vector_addition_advanced() {
    vector *v = vector_from_list(3, (NN_TYPE[]){1, 2, 3});
    vector *w = vector_from_list(3, (NN_TYPE[]){4, 5, 6});
    vector *x = vector_from_list(3, (NN_TYPE[]){7, 8, 9});
    vector *y = vector_from_list(3, (NN_TYPE[]){10, 11, 12});
    vector *z = vector_from_list(3, (NN_TYPE[]){13, 14, 15});

    vector *vw    = vector_addition(v, (number *)w);
    vector *vwx   = vector_addition(vw, (number *)x);
    vector *vwxy  = vector_addition(vwx, (number *)y);
    vector *vwxyz = vector_addition(vwxy, (number *)z);

    test_assert(VECTOR(vwxy, 0) == 35, "xwxy[0] == 40");
    test_assert(VECTOR(vwxy, 1) == 40, "xwxy[1] == 40");
    test_assert(VECTOR(vwxy, 2) == 45, "xwxy[2] == 45");
    test_assert(v == vwxyz, "Addition don't create a copy");

    number_delete(v);
    number_delete(w);
    number_delete(x);
    number_delete(y);
    number_delete(z);

    return 0;
}


int test_vector_index_of() {
    vector *v1 = vector_from_list(3, (NN_TYPE[]){1, 2, 3});
    test_assert(vector_index_of(v1, 2) == 1, "vector_index_of(v1, 2) == 1");
    test_assert(vector_index_of(v1, 3) == 2, "vector_index_of(v1, 3) == 2");
    test_assert(vector_index_of(v1, 4) == -1, "vector_index_of(v1, 4) == -1");
    number_delete(v1);

    return 0;
}

int test_vector_map() {
    vector *v2        = vector_from_list(3, (NN_TYPE[]){1, 2, 3});
    vector *v2_mapped = vector_map(v2, (NN_TYPE(*)(NN_TYPE))sqrt);
    //    test_assert(VECTOR(v2_mapped,0) == 1., "v2_mapped[0] == %f",
    //    VECTOR(v2_mapped, 0));

    return 0;
}

int test_vector_dot_product() {
    vector *v3 = vector_from_list(3, (NN_TYPE[]){1, 2, 3});
    vector *w2 = vector_from_list(3, (NN_TYPE[]){4, 5, 6});
    test_assert(vector_dot_product(v3, w2) == 32,
                "vector_dot_product(v3, w2) == 32");
    number_delete(v3);
    number_delete(w2);

    return 0;
}

int test_vector_shuffle() {
    vector *v = vector_from_list(5, (NN_TYPE[]){1, 2, 3, 4, 5});
    NN_TYPE v_length = vector_length(v);
    vector_shuffle(v);

    test_assert(vector_length(v) == v_length, "Vector length is correct after shuffle");
    test_assert(vector_index_of(v, 1) != -1, "Vector contains 1 after shuffle");
    test_assert(vector_index_of(v, 2) != -1, "Vector contains 2 after shuffle");
    test_assert(vector_index_of(v, 3) != -1, "Vector contains 3 after shuffle");
    test_assert(vector_index_of(v, 4) != -1, "Vector contains 4 after shuffle");
    test_assert(vector_index_of(v, 5) != -1, "Vector contains 5 after shuffle");
    number_delete(v);

    return 0;
}

int test_vector_uniq() {
    vector *v = vector_from_list(5, (NN_TYPE[]){1, 2, 3, 2, 1});
    vector *v_uniq = vector_uniq(v);

    test_assert(v_uniq->length == 3, "Vector length is correct after uniq");
    test_assert(vector_index_of(v, 1) == 0, "Vector contains 1 after uniq");
    test_assert(vector_index_of(v, 2) == 1, "Vector contains 2 after uniq");
    test_assert(vector_index_of(v, 3) == 2, "Vector contains 3 after uniq");
    
    number_delete(v);
    number_delete(v_uniq);

    return 0;
}

int main()
{
    test_vector_create();
    test_vector_clone();
    test_vector_reshape();

    test_vector_unit();
    test_vector_length();

    test_vector_2d();
    test_vector_3d();

    test_vector_addition();
    test_vector_addition_advanced();

    test_vector_index_of();
    test_vector_map();
    test_vector_dot_product();

    test_vector_shuffle();
    test_vector_uniq();

    return 0;
}
