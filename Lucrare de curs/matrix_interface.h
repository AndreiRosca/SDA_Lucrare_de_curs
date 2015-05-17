#ifndef MATRIX_INTERFACE_H_
#define MATRIX_INTERFACE_H_

#include <stdbool.h>
//#include <stdint.h>

typedef struct matrix {

  long double **matrix_ptr;
  int rows;
  int columns;
  long long int matrix_type;

} matrix;

typedef struct matrix_pair {

  matrix first;
  matrix second;

} matrix_pair;

typedef struct matrix_tuple {

  matrix first;
  matrix second;
  matrix third;
  int row_exchanges;

} matrix_tuple;


matrix matrix_allocate_memory(int total_rows, int total_columns);
void matrix_free_memory(matrix *some_matrix);
matrix matrix_addition(matrix first_matrix, matrix second_matrix);
void matrix_self_addition(matrix first_matrix, matrix second_matrix);
matrix matrix_direct_sum(int num_of_matrices, ...);
matrix matrix_scalar_multiplication(matrix some_matrix, long double scalar);
void scalar_matrix_multiplication(matrix some_matrix, long double scalar);
matrix matrix_transposition(matrix some_matrix);
matrix matrix_multiplication(matrix first_matrix, matrix second_matrix);
matrix matrix_power(matrix some_matrix, int power);
matrix matrix_kronecker_product(matrix first_matrix, matrix second_matrix);
matrix matrix_hadamard_product(matrix first, matrix second);
matrix matrix_vectorization(matrix some_matrix);
matrix matrix_kronecker_sum(matrix first_matrix, matrix second_matrix);
matrix matrix_outer_product(matrix first_vector, matrix second_vector);
long double matrix_inner_product(matrix first_vector, matrix second_vector);
long double matrix_frobenius_norm(matrix some_matrix);
long double matrix_frobenius_product(matrix first_matrix, matrix second_matrix);
long double matrix_condition_number(matrix some_matrix);
matrix identity_matrix(int rows);
void matrix_row_switching(matrix some_matrix, int first_row, int second_row);
void matrix_row_multiplication(matrix some_matrix, int row, long double num);
void matrix_row_addition(matrix some_matrix, int first_row, int second_row, long double num); 
void matrix_flip_left_right(matrix some_matrix);
void matrix_flip_up_down(matrix some_matrix);
long double matrix_trace(matrix some_matrix);
long double matrix_determinant_calculation(matrix some_matrix);
matrix matrix_submatrix(matrix some_matrix, int row, int column);
bool matrix_equality(matrix first_matrix, matrix second_matrix); 
matrix matrix_cholesky_decomposition(matrix some_matrix);
long double vector_norm(matrix some_vector);
long double matrix_determinant_using_qr(matrix some_matrix);
long double matrix_lu_determinant(matrix some_matrix);
long double matrix_lup_determinant(matrix some_matrix);
matrix_pair matrix_lu_decomposition(matrix some_matrix);
matrix_tuple matrix_lup_decomposition(matrix some_matrix);
matrix_pair matrix_qr_decomposition(matrix some_matrix);
matrix qr_iteration_for_eigenvalues(matrix some_matrix);
matrix qr_matrix_eigenvectors(matrix some_matrix);
matrix matrix_eigenvector_power_iteration(matrix some_matrix);
matrix_pair matrix_antisymmetric_part(matrix some_matrix);
matrix matrix_adjugate(matrix some_matrix);
matrix matrix_inverse(matrix some_matrix);
matrix system_solve_by_cramer(matrix first_matrix, matrix second_matrix);
matrix system_solve_using_inverse(matrix first_matrix, matrix second_matrix);
matrix solve_system_using_lup_decompozition(matrix coefficients_matrix, matrix right_matrix);
matrix eigenvectors_by_simultaneous_iteration(matrix some_matrix);
matrix_pair matrix_qr_by_givens_rotation(matrix some_matrix);
matrix_pair gram_schmidt_qr_decomposition(matrix some_matrix);
matrix solve_system_using_qr_decomposition(matrix first_matrix, matrix second_matrix);
matrix matrix_exponential(matrix some_matrix);
matrix matrix_square_root(matrix some_matrix);
matrix matrix_sine(matrix some_matrix);
matrix matrix_cosine(matrix some_matrix);
matrix matrix_sign_function(matrix some_matrix);
int matrix_get_rank(matrix some_matrix);


long long int matrix_classify(matrix some_matrix);
void matrix_print_type(long long int matrix_type);
bool is_identity_matrix(matrix some_matrix);
bool is_zero_matrix(matrix some_matrix);
bool is_binary_matrix(matrix some_matrix);
bool is_anti_diagonal_matrix(matrix some_matrix);
bool is_arrowhead_matrix(matrix some_matrix);
bool is_markov_matrix(matrix some_matrix);
bool is_left_stochastic_matrix(matrix some_matrix);
bool is_doubly_stochastic_matrix(matrix some_matrix);
bool is_tridiagonal_matrix(matrix some_matrix);
bool is_pentadiagonal_matrix(matrix some_matrix);
bool is_positive_matrix(matrix some_matrix);
bool is_signature_matrix(matrix some_matrix);
bool is_toeplitz_matrix(matrix some_matrix);
bool is_walsh_matrix(matrix some_matrix);
bool is_vandermonde_matrix(matrix some_matrix);
bool is_hankel_matrix(matrix some_matrix);
bool is_circulant_matrix(matrix some_matrix);
bool is_involutory_matrix(matrix some_matrix);
bool is_alternating_sign_matrix(matrix some_matrix);
bool is_frobenius_matrix(matrix some_matrix);
bool is_metzler_matrix(matrix some_matrix);
bool is_monomial_matrix(matrix some_matrix);
bool is_skew_symmetric_matrix(matrix some_matrix);
bool is_symplectic_matrix(matrix some_matrix);
bool is_idempotent_matrix(matrix some_matrix);
bool is_orthogonal_matrix(matrix some_matrix);
bool is_permutation_matrix(matrix some_matrix);
bool is_symmetric_matrix(matrix some_matrix);
bool is_positive_definite_matrix(matrix some_matrix);
bool is_invertible_matrix(matrix some_matrix);
bool is_sparse_matrix(matrix some_matrix);
bool is_shift_matrix(matrix some_matrix);
bool is_lower_shift_matrix(matrix some_matrix);
bool is_upper_shift_matrix(matrix some_matrix);
bool is_lehmer_matrix(matrix some_matrix);

#endif
