#include "probability.h"
#include "math.h"
#include "matrix.h"
#include "number.h"
#include "string.h"
#include "vector.h"

/**
 * Loops over the columns of a probability space.
 *
 * @param space A pointer to a probability space
 */
#define PROBABILITY_COLUMNS(space) \
    for(size_t column = 0; column < space->samples->columns; column++)

/**
 * Loops over the columns of a probability space and checks if the
 * specified field is equal to the current column's field.
 *
 * @param space A pointer to a probability space
 * @param field The name of a field for which we seek the corresponding column
 */
#define PROBABILITY_COLUMN(space, field) \
    for(size_t column = 0; column < space->samples->columns; column++) \
        if(strcmp(space->fields[column], field) == 0)
/**
 * Loops over the columns of a probability space and iterates over each event
 * in the column's corresponding event vector.
 *
 * @param space A pointer to a probability space
 * @param field The name of a field for which to iterate over events
 */
#define PROBABILITY_FOREACH(space, field) \
    PROBABILITY_COLUMN(space, field) \
        for (size_t index = 0; index < space->events[column]->length; index++)

/**
 * Loops over the columns of a probability space and calculates the sum of
 * a given expression for each event in the specified field's corresponding
 * event vector; the expression is defined using x to represent the value of the event
 * and Px to represent its probability.
 *
 * @param space A pointer to a probability space
 * @param field The name of the field for which to calculate the sum
 * @param accumulator A variable that will hold the sum
 * @param expression An expression, defined using x and Px, to calculate the sum
 */
#define PROBABILITY_SUM(space, field, accumulator, expression) \
    PROBABILITY_FOREACH(space, field) { \
        NN_TYPE x = VECTOR(space->events[column], index); \
        NN_TYPE Px = VECTOR(space->P[column], index); \
        accumulator += expression; \
    }

probability *probability_space_populate(probability *space);
probability *probability_from_matrix(matrix *samples, char **fields) {
    probability *space_ptr;
    size_t space_width_size;
    char **samples_fields;

    space_width_size= sizeof(vector*) * samples->columns;
    samples_fields = malloc(samples->columns * sizeof(char*));

    for(size_t index = 0; index < samples->columns; index++) {
        samples_fields[index] = strdup(fields[index]);
    }

    probability space = {
        .fields = samples_fields,
        .samples = matrix_clone(samples),
        .events = malloc(space_width_size),
        .occurs = malloc(space_width_size),
        .P = malloc(space_width_size),
        .variance = malloc(samples->columns * sizeof(NN_TYPE)),
        .covariance = matrix_create(samples->columns, samples->columns),
        .correlation = matrix_create(samples->columns, samples->columns)
    };

    probability_space_populate(&space);

    space_ptr = malloc(sizeof(probability));
    *space_ptr = space;

    return space_ptr;
}

void probability_delete(probability *space) {
    for(size_t index = 0; index < space->samples->columns; index++) {
        number_delete(space->events[index]);
        number_delete(space->P[index]);
        number_delete(space->occurs[index]);
        free(space->fields[index]);
    }
    free(space->fields);

    number_delete(space->samples);
    number_delete(space->covariance);
    number_delete(space->correlation);

    free(space->events);
    free(space->occurs);
    free(space->P);
    free(space->variance);

    free(space);
}

/**
 * Finds the index of a given field in a probability space's list of fields.
 *
 * @param space A pointer to a probability space
 * @param field The name of the field for which to find the index
 * @returns The index of the field in the list of fields, or 0 if the field is not found
*/
size_t probability_get_field_index(probability *space, char *field) {
    size_t index = 0;
    while(space->fields[index]){
        if(strcmp(space->fields[index], field) == 0) {
            return index;
        }

        index++;
    }

    return 0;
}

/**
 * Counts the number of times each event occurs in a probability space.
 *
 * @param space A pointer to a probability space.
 * @returns The same probability space, but with the `occurs` member set to the frequency of each event.
 */
probability *probability_count_events(probability *space) {
    MATRIX_FOREACH(space->samples) {
        int event_index = vector_index_of(space->events[column], MATRIX(space->samples, row, column));
        VECTOR(space->occurs[column], (size_t)event_index) += 1;
    }

    return space;
}

/**
 * Calculates the joint probability mass of multiple fields taking on specific values.
 *
 * Joint probability mass is the probability of all specified values occurring together in a
 * discrete probability distribution with multiple fields.
 *
 * @param space A pointer to a probability space.
 * @param fields An array of the names of the fields.
 * @param values An array of the values of the fields.
 * @returns The joint probability mass of the specified fields taking on the given values in the probability space.
 */
NN_TYPE probability_mass_and(probability *space, char **fields, NN_TYPE *values) {
    size_t occur = 0;

    for(size_t index = 0; index < space->samples->rows; index++) {
        int is_suitable = 0;

        PROBABILITY_COLUMNS(space) {
            size_t field_index = 0;
            int is_breaked = 0;
            while(fields[field_index]) {
                if(strcmp(space->fields[column], fields[field_index]) == 0) {
                    if(MATRIX(space->samples, index, column) == values[field_index]) {
                        is_suitable = 1;
                    } else {
                        is_suitable = 0;
                        is_breaked = 1;
                        break;
                    }
                }
                field_index++;
            }

            if(is_breaked) {
                is_suitable = 0;
                break;
            }
        }

        if(is_suitable) {
            occur++;
        }
    }

    return (NN_TYPE)occur / (NN_TYPE)space->samples->rows;
}

/**
* Calculates the conditional probability of a value in one field occurring given that
* a value in another field has occurred.
*
* Conditional probability is the probability of an event occurring given that another event has occurred.
* This function calculates the joint probability mass of both fields taking on the specified values
* divided by the probability mass of the specified value in the second field.
*
* @param space A pointer to a probability space.
* @param A_field The name of the field for which to calculate conditional probability.
* @param A_value The value of the field for which to calculate conditional probability.
* @param B_field The name of the field that occurred for calculating conditional probability.
* @param B_value The value of the field, aka occurred, for which to calculate conditional probability.
* @returns The conditional probability of the specified values.
*/
NN_TYPE probability_conditional(probability *space, char *A_field, NN_TYPE A_value, char *B_field, NN_TYPE B_value) {
    char *fields[] = { A_field, B_field, NULL };
    NN_TYPE values[] = { A_value, B_value };

    NN_TYPE P_AB = probability_mass_and(space, fields, values);
    NN_TYPE P_B = probability_mass_of(space, B_field, B_value);

    return P_AB / P_B;
}

/**
 * Calculates the conditional probability of a value in one field occurring given that
 * a value in another field has occurred using Bayes theorem.
 *
 * Bayes theorem is a formula that describes how to update the probabilities of hypotheses
 * when given evidence. It is commonly used in statistical inference and machine learning.
 *
 * @param space A pointer to a probability space.
 * @param A_field The name of the field for which to calculate conditional probability.
 * @param A_value The value of the field for which to calculate conditional probability.
 * @param B_field The name of the field that occurred for calculating conditional probability.
 * @param B_value The value of the field, aka occurred, for which to calculate conditional probability.
 * @returns The conditional probability of the specified values, calculated using Bayes theorem.
 */
NN_TYPE probability_bayes(probability *space, char *A_field, NN_TYPE A_value, char *B_field, NN_TYPE B_value) {
    NN_TYPE P_BA = probability_conditional(space, B_field, B_value, A_field, A_value);
    NN_TYPE P_A = probability_mass_of(space, A_field, A_value);
    NN_TYPE P_B = probability_mass_of(space, B_field, B_value);

    return P_A * P_BA / P_B;
}

/**
 * Calculates the probability mass of a given field taking on a particular value.
 *
 * Probability mass is the probability of a specific value occurring in a discrete probability distribution.
 *
 * @param space A pointer to a probability space.
 * @param field The name of the field.
 * @param value The value of the field for which to calculate the probability mass.
 * @returns The probability mass of the field taking on the given value in the probability space.
 */
NN_TYPE probability_mass_of(probability *space, char *field, NN_TYPE value) {
    PROBABILITY_COLUMN(space, field) {
        return VECTOR(space->P[column], vector_index_of(space->events[column], value));
    }

    return 0;
}

/**
 * Calculates the expected value (aka mean) of a given field in a probability space.
 *
 * Expected value is the sum of the product of values and their probabilities in a probability space.
 *
 * @param space A pointer to a probability space.
 * @param field The name of the field.
 * @returns The expected value of the field in the probability space.
 */
NN_TYPE probability_expected_value(probability *space, char *field)
{
    NN_TYPE mu = 0;

    PROBABILITY_SUM(space, field, mu, x * Px);

    return mu;
}

/**
 * Calculates the variance of a given field in a probability space.
 *
 * Variance is a measure of how spread out a set of data is. Specifically, it measures
 * the average of the squared differences from the mean. A low variance indicates that
 * the data points tend to be closer to the mean, while a high variance indicates that
 * the data points are widely dispersed.
 *
 * @param space A pointer to a probability space.
 * @param field The name of the field.
 * @returns The variance of the field in the probability space.
 */
NN_TYPE probability_variance(probability *space, char *field)
{
    NN_TYPE mu = probability_expected_value(space, field);
    NN_TYPE sigma2 = 0;

    PROBABILITY_SUM(space, field, sigma2, pow(x - mu, 2) * Px);

    return sigma2;
}

/**
 * Calculates the covariance between two specified fields in a probability space.
 *
 * Covariance is a measure of how two variables change together. It measures the extent to which
 * the variables are linearly associated. A positive covariance indicates that the two variables
 * tend to increase or decrease together, while a negative covariance indicates that one variable
 * tends to increase as the other decreases.
 *
 * @param space A pointer to a probability space.
 * @param field The name of the first field.
 * @param related_field The name of the second field.
 * @returns The covariance between the two specified fields in the probability space.
 */
NN_TYPE probability_covariance(probability *space, char *field, char *related_field)
{
    char *fields[] = { field, related_field, NULL };
    vector *origin = space->events[probability_get_field_index(space, field)];
    vector *related = space->events[probability_get_field_index(space, related_field)];

    NN_TYPE mu_field = probability_expected_value(space, field);
    NN_TYPE mu_related = probability_expected_value(space, related_field);

    NN_TYPE covariance = 0;

    //#pragma omp parallel for reduction (+:covariance)
    VECTOR_FOREACH(origin) {
        NN_TYPE x = VECTOR(origin, index);
        NN_TYPE y = VECTOR(related, index);
        NN_TYPE values[] = {x, y};
        NN_TYPE P = probability_mass_and(space, fields, values);
        covariance += (x - mu_field) * (y - mu_related) * P;
    }

    return covariance;
}

/**
 * Calculates the correlation between two specified fields in a probability space.
 *
 * Correlation is a statistical measure that indicates the extent to which two or more variables are related.
 * In probability theory and statistics, correlational analysis often measures the strength of a relationship between
 * two variables, and the direction, which can be positive or negative. It ranges between -1 and 1, where 1 represents a
 * perfect positive correlation and -1 represents a perfect negative correlation, and 0 indicates no correlation.
 *
 * @param space A pointer to a probability space.
 * @param field The name of the first field.
 * @param related_field The name of the second field.
 * @returns The correlation between the two specified fields in the probability space.
 */
NN_TYPE probability_correlation(probability *space, char *field, char *related_field)
{
    size_t field_index = probability_get_field_index(space, field);
    size_t related_field_index = probability_get_field_index(space, related_field);
    NN_TYPE mu_field = space->variance[field_index];
    NN_TYPE mu_related = space->variance[related_field_index];

    NN_TYPE correlation = MATRIX(space->covariance, field_index, related_field_index) / sqrt(mu_field * mu_related);

    return correlation;
}

probability*
probability_space_variance(probability *space) {
    PROBABILITY_COLUMNS(space) {
        number *rows = number_create((NN_TYPE)space->samples->rows);
        space->P[column] = vector_division(vector_clone(space->occurs[column]),
                                          rows);

        space->variance[column] = probability_variance(space, space->fields[column]);

        number_delete(rows);
    }

    return space;
}

probability *probability_space_covariance(probability *space) {
    PROBABILITY_COLUMNS(space) {
        for(size_t related_column = 0; related_column <= column; related_column++) {
            NN_TYPE covariation = probability_covariance(space, space->fields[column], space->fields[related_column]);;

            MATRIX(space->covariance, column, related_column) = covariation;
            MATRIX(space->covariance, related_column, column) = covariation;
        }
    }

    return space;
}

probability *probability_space_correlation(probability *space) {
    PROBABILITY_COLUMNS(space) {
        for(size_t related_column = 0; related_column <= column; related_column++) {
            NN_TYPE correlation = probability_correlation(space, space->fields[column], space->fields[related_column]);
            MATRIX(space->correlation, column, related_column) = correlation;
            MATRIX(space->correlation, related_column, column) = correlation;
        }
    }

    return space;
}

/**
 * Populates a probability space with data from its samples and calculates variance, covariance, and correlation
 * between all fields. Returns a pointer to the modified probability space.
 *
 * @param space A pointer to the probability space to be populated.
 * @returns A pointer to the modified probability space.
 */
probability *probability_space_populate(probability *space) {
    matrix *samples = space->samples;

    PROBABILITY_COLUMNS(space) {
        vector *column_data = matrix_column_vector(samples, column);
        VECTOR_CHECK(column_data);
        space->events[column] = vector_unique(column_data);
        VECTOR_CHECK(space->events[column]);
        space->occurs[column] = vector_create(space->events[column]->length);
        VECTOR_CHECK(space->occurs[column]);

        number_delete(column_data);
    }

    probability_count_events(space);

    probability_space_correlation(
        probability_space_covariance(
            probability_space_variance(space)
        )
    );

    return space;

error:
    return NULL;
}
