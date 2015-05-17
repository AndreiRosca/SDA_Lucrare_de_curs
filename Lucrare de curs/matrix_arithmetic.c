#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "interface.h"
#include "matrix_interface.h"

const char *matrix_menu_items[] = {

  "Matrix addition",
  "Matrix direct sum",
  "Matrix scalar multiplication",
  "Matrix transposition",
  "Matrix multiplication",
  "Matrix power",
  "Determinant calculation (using QR decomposition)",
  "Determinant calculation (using LU decomposition)",
  "Determinant calculation (using LUP decomposition)",
  "Determinant calculation (slow method)",
  "Matrix inverse",
  "Adjugate matrix",
  "Matrix flip left-right",
  "Matrix flip up-down",
  "Kronecker product",
  "Hadamard product",
  "Frobenius product",
  "Kronecker Sum",
  "Matrix outer product", 
  "Solve a system of linear equations (using Cramer's rule)",
  "Solve a system of linear equations (using matrix inverse)",
  "Solve a system of linear equations (using QR decomopsition)",
  "Solve a system of linear equations (using LUP decomopsition)",
  "Create a submatrix",
  "Get Matrix type",
  "Matrix row operations",
  "Matrix eigenvalues",
  "Matrix eigenvectors (using QR algorighm)",
  "Matrix eigenvector (using power iteration)",
  "Get Matrix trace",
  "Matrix vectorization",
  "Matrix LU decomposition",
  "Matrix LUP decomposition",
  "Matrix QR decomposition",
  "Matrix QR decomposition (using Givens rotation)",
  "Matrix Cholesky Decomposition",
  "Matrix exponential",
  "Matrix square root",
  "Matrix sine",
  "Matrix cosine",
  "Matrix Frobenius norm",
  "Matrix condition number",
  "Matrix sign function",
  "Matrix rank",
  "Exit",
  ""
};

enum matrix_operations { 

  matrix_add = 1, direct_sum, scalar_mul, transposition, matrix_mul, matrix_power_calc,
  det_qr, lu_determinant, lup_determinant, slow_det, matrix_inv, 
  adjugate_matr, fliplr_matrix, flipud_matrix, kronecker_matrix_product, 
  hadamard_product, frobenius_product, calc_kronecker_sum, outer_prod,
  cramer_system_of_equations, inv_system_of_equations, qr_system_of_linear_equations, 
  lup_system_of_linear_equations, create_a_submatrix, get_matrix_type, materix_row_operaions, 
  matrix_eigenvalues, matrix_eigenvectors, eigenvector_power_it, 
  calc_matrix_trace, matrix_vectorize, LU_decomposition, LUP_decomposition, QR_decomposition, 
  qr_givens, cholesky_dcmp, matrix_exp, matrix_sqrt, matrix_sine_calc, 
  matrix_cos, frobenius_norm, condition_number, sign_function, /*sylvester_equation,*/ matr_rank, quit

};

int main(int argc, char **argv)
{
  const char *greeting_message = "Hello! Please choose an option:\n";
  char user_input[USER_INPUT_SIZE];
  matrix first_matrix, second_matrix, result_matrix, system_solution;
  int menu_option, exponent, row, col, operation, frow, srow, rank;
  long double det, norm;
  matrix_tuple some_matrices;
  matrix_pair decomposition;
  double scalar;
  bool running = true, silent_mode = false;

  const char *row_operations[] = {

    "Row switching",
    "Row multiplication",
    "Row addition",
    ""
  };

  enum row_oper { switching = 1, multiply, add};

  if (1 == argc) {
    printf("%s\n", greeting_message);
    print_menu(matrix_menu_items);
  }
  else if (2 == argc) {
    
    if (!strcmp(argv[1], "--silent")) {
      silent_mode = true;
    }
    else {
      printf("Unknown command line argument. Programm terminates.\n");
      return EXIT_FAILURE;
    }
  }
  else {
    printf("Unexpected number of arguments. Programm terminates.\n");
    return EXIT_FAILURE;
  }
  
  while (running) {

    if (!silent_mode)
      printf("Enter the command (m for menu): ");

    fgets(user_input, USER_INPUT_SIZE, stdin);
    user_input[0] = tolower(user_input[0]);

    if ('\n' == user_input[0])
      continue;
    else if ('m' == user_input[0]) {

      printf("\n\n");
      print_menu(matrix_menu_items);
      continue;
    }
    else if (NULL == strchr(user_input, '\n')) {

      flush();
    }

    menu_option = 0;
    menu_option = atoi(user_input);
    switch (menu_option) {

    case scalar_mul:

      if (!silent_mode)
	printf("\nEnter your matrix:\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }
      if (!silent_mode)
	printf("\nEnter your scalar: ");
      scanf("%lf", &scalar);
      second_matrix = matrix_scalar_multiplication(first_matrix, scalar);

      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      flush();
      break;

    case matrix_add:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }
      result_matrix = matrix_addition(first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(result_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      matrix_free_memory(&result_matrix);
      printf("\n");
      break;

    case direct_sum:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }
      result_matrix = matrix_direct_sum(2, first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(result_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      matrix_free_memory(&result_matrix);
      printf("\n");
      break;

    case det_qr:

      if (!silent_mode)
	printf("\nEnter your matrix: ");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      det = matrix_determinant_using_qr(first_matrix);

      if (!isnan(det)) {
	printf("\nThe determinant is %Lf\n", det);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case lup_determinant:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      det = matrix_lup_determinant(first_matrix);

      if (!isnan(det)) {
	printf("\nThe determinant is %lf\n", det);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case lu_determinant:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      det = matrix_lu_determinant(first_matrix);

      if (!isnan(det)) {
	printf("\nThe determinant is %lf\n", det);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case get_matrix_type:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      first_matrix.matrix_type = matrix_classify(first_matrix);
      printf("\n");
      matrix_print_type(first_matrix.matrix_type);
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case transposition:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_transposition(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe transpose is:\n\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case cholesky_dcmp:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_cholesky_decomposition(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {
	matrix_print(second_matrix);
	result_matrix = matrix_transposition(second_matrix);
	matrix_print(result_matrix);
	matrix_free_memory(&result_matrix);
	matrix_free_memory(&second_matrix);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case calc_matrix_trace:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      printf("Matrix trace is : %Lf\n", matrix_trace(first_matrix));
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case matrix_mul:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      result_matrix = matrix_multiplication(first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n\n");
	matrix_print(result_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      matrix_free_memory(&result_matrix);
      printf("\n");
      break;

    case matrix_power_calc:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("Enter the exponent: ");
      scanf("%i", &exponent);
      flush();
      second_matrix = matrix_power(first_matrix, exponent);

      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n\n");
	matrix_print(second_matrix);
    }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case hadamard_product:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      result_matrix = matrix_hadamard_product(first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(result_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      matrix_free_memory(&result_matrix);
      printf("\n");
      break;

    case create_a_submatrix:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }
      if (!silent_mode)
	printf("\nEnter the index of the row, you want to delete (-1 if you don't): ");
      scanf("%i", &row);
      if (!silent_mode)
	printf("Enter the index of the column, you want to delete (-1 if you don't): ");
      scanf("%i", &col);
      flush();
      result_matrix = matrix_submatrix(first_matrix, row, col);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(result_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&result_matrix);
      printf("\n");
      break;

    case qr_givens:

      if (!silent_mode)
	printf("\nEnter your matrix:\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      decomposition = matrix_qr_by_givens_rotation(first_matrix);

      if (NULL != decomposition.first.matrix_ptr) {

	printf("\nThe result is:\nQ = \n");
	matrix_print(decomposition.first);
	printf("R = \n");
	matrix_print(decomposition.second);
      }

      matrix_free_memory(&decomposition.first);
      matrix_free_memory(&decomposition.second);
      printf("\n");
      break;

    case QR_decomposition:

      if (!silent_mode)
	printf("\nEnter your matrix:\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      decomposition = matrix_qr_decomposition(first_matrix);

      if (NULL != decomposition.first.matrix_ptr) {

	printf("\nThe result is:\nQ = \n");
	matrix_print(decomposition.first);
	printf("R = \n");
	matrix_print(decomposition.second);
      }

      matrix_free_memory(&decomposition.first);
      matrix_free_memory(&decomposition.second);
      printf("\n");
      break;

    case matrix_eigenvalues:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = qr_iteration_for_eigenvalues(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {

	if (!silent_mode)
	  printf("\nMatrix's real eigenvalues:\n\n");
	matrix_print(second_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case LUP_decomposition:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      some_matrices = matrix_lup_decomposition(first_matrix);

      if ((NULL != some_matrices.first.matrix_ptr) &&
	  (NULL != some_matrices.second.matrix_ptr) &&
	  (NULL != some_matrices.third.matrix_ptr)) {

	printf("\nL:\n");
	matrix_print(some_matrices.first);
	printf("\nU:\n");
	matrix_print(some_matrices.second);
	printf("\nP:\n");
	matrix_print(some_matrices.third);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&some_matrices.first);
      matrix_free_memory(&some_matrices.second);
      matrix_free_memory(&some_matrices.third);
      printf("\n");
      break;

    case LU_decomposition:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      decomposition = matrix_lu_decomposition(first_matrix);

      if ((NULL != decomposition.first.matrix_ptr) &&
	  (NULL != decomposition.second.matrix_ptr)) {

	printf("\nL:\n");
	matrix_print(decomposition.first);
	printf("\nU:\n");
	matrix_print(decomposition.second);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&decomposition.first);
      matrix_free_memory(&decomposition.second);
      printf("\n");
      break;

    case slow_det:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");

      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      det = matrix_determinant_calculation(first_matrix);

      if (!isnan(det)) {
	printf("\nThe determinant is: %Lf\n", det);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case matrix_eigenvectors:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = qr_matrix_eigenvectors(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe eigenvectors are:\n\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case eigenvector_power_it:

      if (!silent_mode)
	printf("\nEnter your matrix:\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_eigenvector_power_iteration(first_matrix);
      if (!silent_mode)
	printf("\nThe eigenvectors are:\n\n");
      matrix_print(second_matrix);
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case matrix_inv:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_inverse(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {

	if (!silent_mode)
	  printf("\nThe inverse of the matrix is :\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case adjugate_matr:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_adjugate(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {

	if (!silent_mode)
	  printf("\nThe matrix adjugate is :\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case lup_system_of_linear_equations:

      if (!silent_mode)
	printf("\nEnter the coefficient's matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter right hand side matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	continue;
      }

      system_solution = solve_system_using_lup_decompozition(first_matrix, second_matrix);

      if (NULL != system_solution.matrix_ptr) {

	if (!silent_mode)
	  printf("\nThe solution of the system is :\n");
	matrix_print(system_solution);
	matrix_free_memory(&system_solution);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case qr_system_of_linear_equations:

      if (!silent_mode)
	printf("\nEnter the coefficient's matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter right hand side matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	continue;
      }

      system_solution = solve_system_using_qr_decomposition(first_matrix, second_matrix);

      if (NULL != system_solution.matrix_ptr) {

	if (!silent_mode)
	  printf("\nThe solution of the system is :\n");
	matrix_print(system_solution);
	matrix_free_memory(&system_solution);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case cramer_system_of_equations:

      if (!silent_mode)
	printf("\nEnter the coefficient's matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter right hand side matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	continue;
      }

      system_solution = system_solve_by_cramer(first_matrix, second_matrix);

      if (NULL != system_solution.matrix_ptr) {

	if (!silent_mode)
	  printf("\nThe solution of the system is :\n");
	matrix_print(system_solution);
	matrix_free_memory(&system_solution);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case inv_system_of_equations:

      if (!silent_mode)
	printf("\nEnter the coefficient's matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter right hand side matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	continue;
      }

      system_solution = system_solve_using_inverse(first_matrix, second_matrix);

      if (NULL != system_solution.matrix_ptr) {

	if (!silent_mode)
	  printf("The solution of the system is :\n");
	matrix_print(system_solution);
	matrix_free_memory(&system_solution);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case fliplr_matrix:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      matrix_flip_left_right(first_matrix);
      if (!silent_mode)
	printf("The result is :\n");
      matrix_print(first_matrix);
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case flipud_matrix:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      matrix_flip_up_down(first_matrix);
      if (!silent_mode)
	printf("The result is :\n");
      matrix_print(first_matrix);
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case matrix_exp:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_exponential(first_matrix);
      
      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("The result is :\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case matrix_sqrt:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_square_root(first_matrix);
      
      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("The result is :\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case matrix_sine_calc:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_sine(first_matrix);
      
      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("The result is :\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case matrix_cos:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_cosine(first_matrix);
      
      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("The result is :\n");
	matrix_print(second_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case outer_prod:

      if (!silent_mode)
	printf("\nEnter first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      result_matrix = matrix_outer_product(first_matrix, second_matrix);
      
      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("The result is :\n");
	matrix_print(result_matrix);
	matrix_free_memory(&result_matrix);
      }
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case frobenius_norm:

      if (!silent_mode)
	printf("\nEnter your matrix: ");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      norm = matrix_frobenius_norm(first_matrix);

      if (!isnan(det)) {
	printf("\nMatrix frobenius norm %Lf\n", norm);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case condition_number:

      if (!silent_mode)
	printf("\nEnter your matrix: ");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      norm = matrix_condition_number(first_matrix);

      if (!isnan(det)) {
	printf("\nMatrix's Condition Number is %Lf\n", norm);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case kronecker_matrix_product:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      result_matrix = matrix_kronecker_product(first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(result_matrix);
	matrix_free_memory(&result_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case calc_kronecker_sum:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      result_matrix = matrix_kronecker_sum(first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(result_matrix);
	matrix_free_memory(&result_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case frobenius_product:

      if (!silent_mode)
	printf("\nEnter the first matrix:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter the second matrix:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      norm = matrix_frobenius_product(first_matrix, second_matrix);

      if (NULL != result_matrix.matrix_ptr) {
	printf("\nThe result is: %Lf\n", norm);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case matrix_vectorize:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");

      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_vectorization(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(second_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

    case materix_row_operaions:

      if (!silent_mode) {
	printf("\nWhat row operation do you want? Available options:\n\n");
	print_menu(row_operations);
	printf(">>");
      }

      scanf("%i", &operation);
      flush();

      if ((operation < switching) || (operation > add)) {
	continue;
      }

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");

      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }
 
      switch (operation) {

      case switching:

	if (!silent_mode)
	  printf("\nFirst row: ");

	scanf("%i", &frow);

	if (!silent_mode)
	  printf("\nSecond row: ");

	scanf("%i", &srow);
	flush();
	matrix_row_switching(first_matrix, frow, srow);
	break;

      case multiply:

	if (!silent_mode)
	  printf("\nRow number: ");

	scanf("%i", &row);

	if (!silent_mode)
	  printf("\nEnter the factor: ");

	scanf("%lf", &scalar);
	flush();
	matrix_row_multiplication(first_matrix, row, scalar);
	break;

      case add:

	if (!silent_mode)
	  printf("\nFirst row: ");

	scanf("%i", &frow);

	if (!silent_mode)
	  printf("\nSecond row: ");

	scanf("%i", &srow);

	if (!silent_mode)
	  printf("\nEnter the factor: ");

	scanf("%lf", &scalar);
	flush();
	matrix_row_addition(first_matrix, frow, srow, scalar);
	break;

      default:
	printf("\nSorry, wrong option -- try again.\n");
	break;
      }
   
      if (!silent_mode)
	printf("\nThe result is:\n");
      matrix_print(first_matrix);
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case sign_function:

      if (!silent_mode)
	printf("\nEnter your matrix:\n\n");

      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      second_matrix = matrix_sign_function(first_matrix);

      if (NULL != second_matrix.matrix_ptr) {
	if (!silent_mode)
	  printf("\nThe result is:\n");
	matrix_print(second_matrix);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      printf("\n");
      break;

      /*    case sylvester_equation:

      printf("\nEnter matrix A:\n\n");
      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      printf("\nEnter matrix B:\n\n");
      second_matrix = matrix_read();

      if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	continue;
      }

      printf("\nEnter matrix C:\n\n");
      result_matrix = matrix_read();

      if (NULL == result_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	matrix_free_memory(&second_matrix);
	continue;
      }

      system_solution = solve_sylvester_equation(first_matrix, second_matrix, result_matrix);

      if (NULL != system_solution.matrix_ptr) {
	printf("\nThe result is:\n");
	matrix_print(system_solution);
      }

      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      matrix_free_memory(&result_matrix);
      matrix_free_memory(&system_solution);
      printf("\n");
      break;
      */

    case matr_rank:

      if (!silent_mode)
	printf("\nEnter your matrix: ");

      first_matrix = matrix_read();

      if (NULL == first_matrix.matrix_ptr) {
	continue;
      }

      rank = matrix_get_rank(first_matrix);

      if (-1 != rank) {
	printf("\nThe rank of the matrix is %i\n", rank);
      }
      matrix_free_memory(&first_matrix);
      printf("\n");
      break;

    case quit:

      if (!silent_mode)
	printf("\nGoodbye!\n\n");

      running = false;
      break;

    default:

      printf("\nSorry, wrong option -- try again.\n");
      break;
    }

  }

  return EXIT_SUCCESS;
}
