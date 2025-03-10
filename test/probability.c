#include "probability.h"
#include "matrix.h"
#include "vector.h"
#include <math.h>
#include <nn.h>
#include <string.h>
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

#define test_assert(test, message, ...)                                        \
    if (!(test)) {                                                             \
        log_error(message, ##__VA_ARGS__);                                     \
        return 1;                                                              \
    } else {                                                                   \
        log_ok(message, ##__VA_ARGS__);                                        \
    }


int test_probability_from_matrix()
{
    matrix *m = matrix_create(2, 3);
    test_assert(m, "Matrix m allocated");
    MATRIX(m, 0, 0) = 0.1;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 0, 2) = 0.5;
    MATRIX(m, 1, 0) = 0.4;
    MATRIX(m, 1, 1) = 0.3;
    MATRIX(m, 1, 2) = 0.7;

    const char *fields[] = {"field1\0", "field2\0", "field3\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(strcmp(p->fields[2], fields[2]) == 0,
                "Fields match at index 2");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 0, 2) == MATRIX(m, 0, 2),
                "Matrix samples found at 0,2");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 1, 2) == MATRIX(m, 1, 2),
                "Matrix samples found at 1,2");

    // Add more tests as needed
    printf("All tests passed\n");

    // Clean up
    number_delete(m);
    probability_delete(p);

error:
    return 0;
}

int test_probability_mass_of()
{
    matrix *m = matrix_create(2, 3);
    test_assert(m, "Matrix m allocated");
    MATRIX(m, 0, 0) = 0.1;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 0, 2) = 0.5;
    MATRIX(m, 1, 0) = 0.4;
    MATRIX(m, 1, 1) = 0.3;
    MATRIX(m, 1, 2) = 0.7;

    const char *fields[] = {"field1\0", "field2\0", "field3\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(strcmp(p->fields[2], fields[2]) == 0,
                "Fields match at index 2");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 0, 2) == MATRIX(m, 0, 2),
                "Matrix samples found at 0,2");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 1, 2) == MATRIX(m, 1, 2),
                "Matrix samples found at 1,2");

    // Add more tests as needed
    printf("All tests passed\n");

    /* Test probability mass of some field's value for the probability space */
    
    float pm = probability_mass_of(p, (char *)fields[2], 0.5);
    test_assert(fabs(pm - 0.5) < 1e-7, "Probability mass of field value is correct.");

    // Clean up
    number_delete(m);
    probability_delete(p);

error:
    return 0;
}

int test_probability_expected_value()
{
    matrix *m = matrix_create(2, 3);
    test_assert(m, "Matrix m allocated");

    MATRIX(m, 0, 0) = 0.1;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 0, 2) = 0.5;
    MATRIX(m, 1, 0) = 0.4;
    MATRIX(m, 1, 1) = 0.3;
    MATRIX(m, 1, 2) = 0.7;

    const char *fields[] = {"field1\0", "field2\0", "field3\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(strcmp(p->fields[2], fields[2]) == 0,
                "Fields match at index 2");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 0, 2) == MATRIX(m, 0, 2),
                "Matrix samples found at 0,2");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 1, 2) == MATRIX(m, 1, 2),
                "Matrix samples found at 1,2");

    // Check expected value of the probability space
    float expected_value = probability_expected_value(p, (char *)fields[2]);
    float expected_value_true = (0.5 + 0.7) / 2;
    test_assert(fabs(expected_value - expected_value_true) < 1e-7,
                "Expected value is correct for field3.");

    // Clean up
    number_delete(m);
    probability_delete(p);

error:
    return 0;
}

int test_probability_variance()
{
    matrix* m = matrix_create(2, 3);
    test_assert(m, "Matrix m allocated");

    MATRIX(m, 0, 0) = 0.1;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 0, 2) = 0.5;
    MATRIX(m, 1, 0) = 0.4;
    MATRIX(m, 1, 1) = 0.3;
    MATRIX(m, 1, 2) = 0.7;

    const char *fields[] = {"field1\0", "field2\0", "field3\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(strcmp(p->fields[2], fields[2]) == 0,
                "Fields match at index 2");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 0, 2) == MATRIX(m, 0, 2),
                "Matrix samples found at 0,2");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 1, 2) == MATRIX(m, 1, 2),
                "Matrix samples found at 1,2");

    // Check variance of the probability space
    float variance = probability_variance(p, (char*)fields[2]);
    float variance_true = ((0.5 - 0.6)*(0.5 - 0.6) + (0.7 - 0.6)*(0.7 - 0.6)) / 2;
    test_assert(fabs(variance - variance_true) < 1e-7, "Variance is calculated correctly for field3");

    // Clean up
    number_delete(m);
    probability_delete(p);

    error:
    return 0;
}

int test_probability_covariance()
{
    matrix *m = matrix_create(3, 2);
    test_assert(m, "Matrix m allocated");

    MATRIX(m, 0, 0) = 0.8;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 1, 0) = 0.2;
    MATRIX(m, 1, 1) = 0.4;
    MATRIX(m, 2, 0) = 0.5;
    MATRIX(m, 2, 1) = 0.7;

    const char *fields[] = {"field1\0", "field2\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 2, 0) == MATRIX(m, 2, 0),
                "Matrix samples found at 2,0");
    test_assert(MATRIX(p->samples, 2, 1) == MATRIX(m, 2, 1),
                "Matrix samples found at 2,1");

    // Test the covariance value for the probability space
    NN_TYPE cov = probability_covariance(p, (char *)fields[0], (char *)fields[1]);
    NN_TYPE cov_true = ((0.8 - 0.44) * (0.2 - 0.44) + (0.2 - 0.44) * (0.4 - 0.44) + (0.5 - 0.44) * (0.7 - 0.44)) / 3;
    test_assert(fabs(cov - cov_true) < 1e-3,
                "Covariance calculation is correct for fields 1 and 2. %f = %f", cov, cov_true);

    // Clean up
    number_delete(m);
    probability_delete(p);

    error:
        return 0;
}

int test_probability_correlation()
{
    matrix *m = matrix_create(3, 2);
    test_assert(m, "Matrix m allocated");

    MATRIX(m, 0, 0) = 0.8;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 1, 0) = 0.2;
    MATRIX(m, 1, 1) = 0.4;
    MATRIX(m, 2, 0) = 0.5;
    MATRIX(m, 2, 1) = 0.7;

    const char *fields[] = {"field1\0", "field2\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 2, 0) == MATRIX(m, 2, 0),
                "Matrix samples found at 2,0");
    test_assert(MATRIX(p->samples, 2, 1) == MATRIX(m, 2, 1),
                "Matrix samples found at 2,1");

    // Test the correlation value for the probability space
    NN_TYPE correlation = probability_correlation(p, (char *)fields[0], (char *)fields[1]);
    NN_TYPE correlation_true = (((0.8 - 0.44) * (0.2 - 0.44) + (0.2 - 0.44) * (0.4 - 0.44) + (0.5 - 0.44) * (0.7 - 0.44)) / 3) / (sqrtf((((0.8 - 0.44) * (0.8 - 0.44) + (0.2 - 0.44) * (0.2 - 0.44) + (0.5 - 0.44) * (0.5 - 0.44)) / 3)) * sqrtf((((0.2 - 0.44) * (0.2 - 0.44) + (0.4 - 0.44) * (0.4 - 0.44) + (0.7 - 0.44) * (0.7 - 0.44)) / 3)));
    test_assert(fabs(correlation - correlation_true) < 1e-2,
                "Correlation calculation is correct for fields 1 and 2. %f = %f", correlation, correlation_true);

    // Clean up
    number_delete(m);
    probability_delete(p);

    error:
        return 0;
}
// Test the conditional probability value for the probability space
int test_probability_conditional()
{
    matrix *m = matrix_create(3, 2);
    test_assert(m, "Matrix m allocated");

    MATRIX(m, 0, 0) = 0.8;
    MATRIX(m, 0, 1) = 0.2;
    MATRIX(m, 1, 0) = 0.2;
    MATRIX(m, 1, 1) = 0.4;
    MATRIX(m, 2, 0) = 0.5;
    MATRIX(m, 2, 1) = 0.7;

    const char *fields[] = {"field1\0", "field2\0"};
    probability *p = probability_from_matrix(m, (char **)fields);

    test_assert(p != NULL, "Probability p is not NULL");
    test_assert(strcmp(p->fields[0], fields[0]) == 0,
                "Fields match at index 0");
    test_assert(strcmp(p->fields[1], fields[1]) == 0,
                "Fields match at index 1");
    test_assert(MATRIX(p->samples, 0, 0) == MATRIX(m, 0, 0),
                "Sample matches at 0,0");
    test_assert(MATRIX(p->samples, 0, 1) == MATRIX(m, 0, 1),
                "Matrix samples found at 0,1");
    test_assert(MATRIX(p->samples, 1, 0) == MATRIX(m, 1, 0),
                "Matrix samples found at 1,0");
    test_assert(MATRIX(p->samples, 1, 1) == MATRIX(m, 1, 1),
                "Matrix samples found at 1,1");
    test_assert(MATRIX(p->samples, 2, 0) == MATRIX(m, 2, 0),
                "Matrix samples found at 2,0");
    test_assert(MATRIX(p->samples, 2, 1) == MATRIX(m, 2, 1),
                "Matrix samples found at 2,1");

    // Test the conditional probability value for field1 and field2
    // Choose values for A and B
    NN_TYPE A_value = 0.8;
    NN_TYPE B_value = 0.2;

    // Choose fields for A and B
    char* A_field = "field1\0";
    char* B_field = "field2\0";

    // Test the conditional probability value for field1 and field2 at A_value and B_value respectively
    // formula: P(B|A) = P(A and B) / P(A)
    // Here, P(A and B) is the probability that values A_value and B_value occur together, which is calculated using probability_joint(), 
    // and P(A) is the probability that A_value occurs without considering B_value
    // Formula for P(A): Sum of probability_joint() for all samples where A_value occurs
    // conditional_probability stores P(B|A)
    NN_TYPE conditional_probability = probability_conditional(p, A_field, A_value, B_field, B_value);

    // Calculate the true conditional probability value using the formula learned in statistics
    // Formula: (P(B) - P(A)*P(B)) / sqrt((P(A)*(1-P(A)))(P(B)*(1-P(B))))
    // Here, P(A) is the probability of A_value, P(B) is the probability of B_value, and P(A and B) is the probability of A_value and B_value
    // The true conditional probability value is stored in cond_true
    NN_TYPE cond_true = (0.2 - 0.44) / sqrtf((0.8 - 0.44)*(0.8 - 0.44) + (0.2 - 0.44)*(0.2 - 0.44) + (0.5 - 0.44)*(0.5 - 0.44));

    // Test if the calculated and true conditional probabilities are similar within a tolerance using test_assert()
    // Print A_value and B_value in the test message
    test_assert(fabs(conditional_probability - cond_true) < 1e-3, 
                "Conditional probability calculation is correct for fields 1 and 2 with A=%f, B=%f.", conditional_probability, cond_true);
    // Clean up
    number_delete(m);
    probability_delete(p);

    error:
        return 0; 
}

int main() { 
    test_probability_from_matrix(); 
    test_probability_mass_of();
    test_probability_expected_value();
    test_probability_variance();
    test_probability_covariance();
    test_probability_correlation();
   // test_probability_conditional();
}
