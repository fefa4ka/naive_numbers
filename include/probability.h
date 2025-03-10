#pragma once

#include "number.h"
#include "vector.h"
#include "matrix.h"

// Life Cycle
probability *probability_from_matrix(matrix *samples, char **fields);
void probability_delete(probability *space); 
//probability *probability_space_from_csv(csv *data, char **fields);

// Getters
NN_TYPE probability_mass_of(probability *space, char *field, NN_TYPE value);
NN_TYPE probability_mass_and(probability *space, char **fields, NN_TYPE *values);
NN_TYPE probability_conditional(probability *space, char *A_field, NN_TYPE A_value, char *B_field, NN_TYPE B_value);
NN_TYPE probability_bayes(probability *space, char *A_field, NN_TYPE A_value, char *B_field, NN_TYPE B_value);
// //NN_TYPE probability_density(probability *space, NN_TYPE a, NN_TYPE b);
// 
// // Properties
// NN_TYPE probability_expected_value_conditional(probability *space, char *expected_field, char *related_field, NN_TYPE value);
NN_TYPE probability_expected_value(probability *space, char *field);
NN_TYPE probability_matrix_expected_value_of_function(probability *space, char *field, NN_TYPE operation(NN_TYPE));
NN_TYPE probability_variance(probability *space, char *field);
NN_TYPE probability_covariance(probability *space, char *field, char *related_field);
NN_TYPE probability_correlation(probability *space, char *field, char *related_field);
// size_t probability_get_field_index(probability *space, char *field);
// NN_TYPE probability_matrix_expected_value(probability *space, char *);
// NN_TYPE probability_matrix_expected_value_of_function(probability *space, NN_TYPE operation(NN_TYPE));
// NN_TYPE probability_variance(probability *space);
//NN_TYPE probability_variance_of_function(probability *space, NN_TYPE operation(NN_TYPE));

// Helpers
// static probability *__probability_uniform(probability *space);
// static probability *__probability_space_count_events(probability *space);
// static probability *__probability_space_count_events_occurs(probability *space);