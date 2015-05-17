#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "interface.h"
#include "matrix_interface.h"

matrix matrix_allocate_memory(int total_rows, int total_columns)
{
  long double **some_matrix = NULL;
  matrix result_matrix = { .rows = total_rows, .columns = total_columns,
			   .matrix_type = 0LL, .matrix_ptr = NULL };
   int row;

  some_matrix = (long double **) calloc(total_rows, sizeof(*some_matrix));

  if (NULL == some_matrix) {
    return result_matrix;
  }

  result_matrix.matrix_ptr = some_matrix;

  for (row = 0; row < total_rows; ++row) {

    some_matrix[row] = (long double *) calloc(total_columns, 
						 sizeof(**some_matrix));
    if (NULL == some_matrix[row]) {

      matrix_free_memory(&result_matrix);
      result_matrix.matrix_ptr = NULL;
      break;
    }
  }

  return result_matrix;
}

void matrix_free_memory(matrix *some_matrix)
{
  int row;

  if ((NULL != some_matrix) && (NULL == some_matrix->matrix_ptr)) {
    return;
  }
  else if (NULL == some_matrix) {
    return;
  }

  for (row = 0; row < some_matrix->rows; ++row) {

    if (NULL == some_matrix->matrix_ptr[row]) {
      break;
    }

    free(some_matrix->matrix_ptr[row]);
  }

  free(some_matrix->matrix_ptr);
  some_matrix->matrix_ptr = NULL;
}

void matrix_row_multiplication(matrix some_matrix, int row, long double num)
{
  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  for (int col = 0; col != some_matrix.columns; ++col) {

    some_matrix.matrix_ptr[row - 1][col] *= num;
  }
}

void matrix_row_addition(matrix some_matrix, int first_row, 
			 int second_row, long double scalar)
{
  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  for (int col = 0; col != some_matrix.columns; ++col) {

    some_matrix.matrix_ptr[first_row - 1][col] = 
      some_matrix.matrix_ptr[first_row - 1][col] +
      some_matrix.matrix_ptr[second_row - 1][col] * scalar;
  }
}

void matrix_row_switching(matrix some_matrix, int first_row, int second_row)
{
  long double matrix_item;

  if (NULL == some_matrix.matrix_ptr) {
    return;
  }
  else if (first_row == second_row) {
    return;
  }

  for (int col = 0; col != some_matrix.columns; ++col) {

    matrix_item = some_matrix.matrix_ptr[first_row - 1][col];
    some_matrix.matrix_ptr[first_row - 1][col] = 
      some_matrix.matrix_ptr[second_row - 1][col];

    some_matrix.matrix_ptr[second_row - 1][col] = matrix_item;
  }
}

void matrix_flip_left_right(matrix some_matrix)
{
  long double matrix_item;
  int row, col;

  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  for (row = 0; row != some_matrix.rows; ++row) {

    for (col = 0; col != (some_matrix.columns / 2); ++col) {

      matrix_item = some_matrix.matrix_ptr[row][col];
      some_matrix.matrix_ptr[row][col] = 
	some_matrix.matrix_ptr[row][some_matrix.columns - col - 1];
      some_matrix.matrix_ptr[row][some_matrix.columns - col - 1] = matrix_item;
    }
  }
}

void matrix_flip_up_down(matrix some_matrix)
{
  long double matrix_item;
  int row, col;

  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  for (col = 0; col != some_matrix.columns; ++col) {

    for (row = 0; row != (some_matrix.rows / 2); ++row) {

      matrix_item = some_matrix.matrix_ptr[row][col];
      some_matrix.matrix_ptr[row][col] = 
	some_matrix.matrix_ptr[some_matrix.rows - row - 1][col];
      some_matrix.matrix_ptr[some_matrix.rows - row - 1][col] = matrix_item;
    }
  }
}

void matrix_self_addition(matrix first_matrix, matrix second_matrix)
{
  int row, column;

  if ((NULL == first_matrix.matrix_ptr) || (NULL == second_matrix.matrix_ptr)) {
    return;
  }
  else if ((first_matrix.rows != second_matrix.rows) || 
	   (first_matrix.columns != second_matrix.columns)) {
    perror("Warning, matrix dimensions do not match (matrix_self_addition).\n");
    return;
  }

  for (row = 0; row < first_matrix.rows; ++row) {

    for (column = 0; column < second_matrix.columns; ++column) {

      first_matrix.matrix_ptr[row][column] +=
	second_matrix.matrix_ptr[row][column];
    }
  }
}

matrix matrix_addition(matrix first_matrix, matrix second_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL, .rows = first_matrix.rows,
			   .columns = first_matrix.columns, .matrix_type = 0 };
  int row, column;

  if ((NULL == first_matrix.matrix_ptr) || (NULL == second_matrix.matrix_ptr)) {
    return result_matrix;
  }
  else if ((first_matrix.rows != second_matrix.rows) || 
	   (first_matrix.columns != second_matrix.columns)) {
    perror("Warning, matrix dimensions do not match (matrix_addition).\n");
    return result_matrix;
  }

  result_matrix = matrix_allocate_memory(first_matrix.rows, 
					 first_matrix.columns);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Sorry, memory allocation failed (matrix_addition).\n");
    return result_matrix;
  }

  for (row = 0; row < first_matrix.rows; ++row) {

    for (column = 0; column < second_matrix.columns; ++column) {

      result_matrix.matrix_ptr[row][column] = 
	first_matrix.matrix_ptr[row][column] + 
	second_matrix.matrix_ptr[row][column];
    }
  }

  return result_matrix;
}

matrix matrix_direct_sum(int num_of_matrices, ...)
{
  matrix direct_sum = { .matrix_ptr = NULL }, current_matrix;
  va_list args;
  int i, row, col, row_offset = 0, col_offset = 0, num_of_rows = 0, num_of_columns = 0;

  va_start(args, num_of_matrices);

  for (i = 0; i != num_of_matrices; ++i) {

    current_matrix = va_arg(args, matrix);

    num_of_rows += current_matrix.rows;
    num_of_columns += current_matrix.columns;
  }

  va_end(args);
  va_start(args, num_of_matrices);

  direct_sum = matrix_allocate_memory(num_of_rows, num_of_columns);

  if (NULL == direct_sum.matrix_ptr) {
    perror("Sorry, can't allocate enough memory (matrix_direct_sum).\n");
    return direct_sum;
  }

  for (i = 0; i != num_of_matrices; ++i) {

    current_matrix = va_arg(args, matrix);

    for (row = 0; row != current_matrix.rows; ++row) {

      for (col = 0; col != current_matrix.columns; ++col) {

	direct_sum.matrix_ptr[row_offset + row][col_offset + col] = current_matrix.matrix_ptr[row][col];
      }
    }

    row_offset += current_matrix.rows;
    col_offset += current_matrix.columns;
  }

  va_end(args);

  return direct_sum;
}

matrix matrix_scalar_multiplication(matrix some_matrix, long double scalar)
{
  matrix result_matrix = { .rows = some_matrix.rows, 
      .columns = some_matrix.columns, .matrix_type = 0, .matrix_ptr = NULL };
  int row, column;

  if (NULL == some_matrix.matrix_ptr) {

    return result_matrix;
  }

  result_matrix = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Sorry, memory allocation failed (matrix_scalar_multiplication).\n");
    return result_matrix;
  }

  for (row = 0; row < some_matrix.rows; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      result_matrix.matrix_ptr[row][column] = 
	some_matrix.matrix_ptr[row][column] * scalar;
    }
  }

  return result_matrix;
}

void scalar_matrix_multiplication(matrix some_matrix, long double scalar)
{
  int row, column;

  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  for (row = 0; row < some_matrix.rows; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

	some_matrix.matrix_ptr[row][column] *= scalar;
    }
  }
}

matrix matrix_adjugate(matrix some_matrix)
{
  matrix adjugate = { .matrix_ptr = NULL }, current_submatrix;
  int row, col;

  if (NULL == some_matrix.matrix_ptr) {
    return adjugate;
  }
  else if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix (matrix_adjugate).\n");
    return adjugate;
  }

  adjugate = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == adjugate.matrix_ptr) {
    perror("Sorry, memory allocation failed (matrix_adjugate).\n");
    return adjugate;
  }

  for (row = 0; row != some_matrix.rows; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      current_submatrix = matrix_submatrix(some_matrix, 1 + row, 1 + col);
	  
      if (NULL == current_submatrix.matrix_ptr) {
	matrix_free_memory(&adjugate);
	return adjugate;
      }

      if (current_submatrix.rows <= 2) {
	adjugate.matrix_ptr[row][col] = matrix_determinant_calculation(current_submatrix);
      }
      else {

	adjugate.matrix_ptr[row][col] = matrix_determinant_using_qr(current_submatrix);

	if (isnan(adjugate.matrix_ptr[row][col])) {
	  matrix_free_memory(&adjugate);
	  matrix_free_memory(&current_submatrix);
	  return adjugate;
	}
      }
      adjugate.matrix_ptr[row][col] *= powl(-1.0L, (long double )row + col);
      matrix_free_memory(&current_submatrix);
    }
  }

  current_submatrix = matrix_transposition(adjugate);
  matrix_free_memory(&adjugate);
  adjugate = current_submatrix;
  
  return adjugate;
}

matrix system_solve_using_inverse(matrix first_matrix, matrix second_matrix)
{
  matrix solution = { .matrix_ptr = NULL }, inverse;

  if (first_matrix.rows != first_matrix.columns) {
    perror("Warning, not a square coefficient's matrix (system_solve_using_inverse).\n");
    return solution;
  }
  else if (second_matrix.columns != 1) {
    perror("Warning, right hand side matrix isn't a column matrix (system_solve_using_inverse).\n");
    return solution;
  }
  else if (first_matrix.rows != second_matrix.rows) {
    perror("Warning, matrix dimensions (coefficient's and rhs) don't match (system_solve_using_inverse).\n");
    return solution;
  }

  inverse = matrix_inverse(first_matrix);

  if (NULL == inverse.matrix_ptr) {
    perror("Warning, matrix inversion failure (system_solve_using_inverse).\n");
    return solution;
  }

  solution = matrix_multiplication(inverse, second_matrix);
  matrix_free_memory(&inverse);

  return solution;
}

matrix system_solve_by_cramer(matrix first_matrix, matrix second_matrix)
{
  matrix solution = { .matrix_ptr = NULL }, restore_vector;
  long double main_det, current_det;
  int row, col;

  if (first_matrix.rows != first_matrix.columns) {
    perror("Warning, not a square coefficient's matrix (system_solve_by_cramer).\n");
    return solution;
  }
  else if (second_matrix.columns != 1) {
    perror("Warning, right hand side matrix isn't a column matrix (system_solve_by_cramer).\n");
    return solution;
  }
  else if (first_matrix.rows != second_matrix.rows) {
    perror("Warning, matrix dimensions (coefficient's and rhs) don't match (system_solve_by_cramer).\n");
    return solution;
  }

  main_det = matrix_determinant_calculation(first_matrix);

  if ((0.0L == main_det) || isnan(main_det)) {
    perror("Sorry, can't solve the system (determinant is zero?) (system_solve_by_cramer).\n");
    return solution;
  }

  solution = matrix_allocate_memory(second_matrix.rows, second_matrix.columns);

  if (NULL == solution.matrix_ptr) {
    perror("Warning, memory allocation failure (system_solve_by_cramer).\n");
    return solution;
  }

  restore_vector = matrix_allocate_memory(second_matrix.rows, second_matrix.columns);

  if (NULL == restore_vector.matrix_ptr) {
    perror("Warning, memory allocation failure (system_solve_by_cramer).\n");
    return solution;
  }

  for (col = 0; col != second_matrix.rows; ++col) {

    for (row = 0; row != second_matrix.rows; ++row) {

      restore_vector.matrix_ptr[row][0] = first_matrix.matrix_ptr[row][col];
      first_matrix.matrix_ptr[row][col] = second_matrix.matrix_ptr[row][0];
    }

    current_det = matrix_determinant_calculation(first_matrix);

    if (isnan(current_det)) {
      matrix_free_memory(&solution);
      matrix_free_memory(&restore_vector);
      perror("Warning, invalid determinant value (system_solve_by_cramer).\n");
      return (matrix) { .matrix_ptr = NULL };
    }
    else {

      solution.matrix_ptr[col][0] = current_det / main_det;
    }

    for (row = 0; row != second_matrix.rows; ++row) {

      first_matrix.matrix_ptr[row][col] = restore_vector.matrix_ptr[row][0];
    }
  }

  matrix_free_memory(&restore_vector);

  return solution;
}

matrix_pair matrix_antisymmetric_part(matrix some_matrix)
{
  matrix_pair decomposition = { .first.matrix_ptr = NULL, .second.matrix_ptr = NULL };
  matrix transpose;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return decomposition;
  }

  transpose = matrix_transposition(some_matrix);
  
  if (NULL == transpose.matrix_ptr) {
    return decomposition;
  }

  decomposition.first = matrix_addition(some_matrix, transpose);

  if (NULL == decomposition.first.matrix_ptr) {
    matrix_free_memory(&transpose);
    return decomposition;
  }

  decomposition.second = matrix_addition(some_matrix, transpose);

  if (NULL == decomposition.second.matrix_ptr) {
    matrix_free_memory(&transpose);
    matrix_free_memory(&decomposition.first);
    return decomposition;
  }

  scalar_matrix_multiplication(decomposition.first, 1.0L / 2);
  scalar_matrix_multiplication(decomposition.second, -1.0L / 2);

  return decomposition;
}

matrix matrix_transposition(matrix some_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  int row, column;

  if (NULL == some_matrix.matrix_ptr) {

    return result_matrix;
  }

  result_matrix = matrix_allocate_memory(some_matrix.columns, some_matrix.rows);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return result_matrix;
  }

  for (row = 0; row < some_matrix.rows; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      result_matrix.matrix_ptr[column][row] = 
	some_matrix.matrix_ptr[row][column];
    }
  }

  return result_matrix;
}

matrix strassen_matrix_multiplication(matrix first_matrix, matrix second_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL };

  return result_matrix;
}

matrix matrix_multiplication(matrix first_matrix, matrix second_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  long double sum;
  int row, column, col_index;

  if ((NULL == first_matrix.matrix_ptr) || NULL == (second_matrix.matrix_ptr)) {
    perror("Warning, bad operands.\n");
    return result_matrix;
  }
  else if (first_matrix.columns != second_matrix.rows) {
    perror("Warning, matrix dimensions do not match.\n");
    return result_matrix;
  }

  result_matrix = matrix_allocate_memory(first_matrix.rows, 
					 second_matrix.columns);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Warning, memory allocation failre.\n");
    return result_matrix;
  }

  for (row = 0; row < first_matrix.rows; ++row) {

    for (column = 0; column < second_matrix.columns; ++column) {

      sum = 0.0L;
      for (col_index = 0; col_index != first_matrix.columns; ++col_index) {

	sum += first_matrix.matrix_ptr[row][col_index] * 
	  second_matrix.matrix_ptr[col_index][column];
      }

      result_matrix.matrix_ptr[row][column] = sum;
    }
  }

  return result_matrix;
}

matrix matrix_power(matrix some_matrix, int power)
{
  matrix result_matrix = { .matrix_ptr = NULL }, old_matrix;
  int iterator;

  if (power < 0) {

    power = -power;
  }

  if (some_matrix.columns != some_matrix.rows) {
    perror("Warning, not a square matrix!.\n");
    return result_matrix;
  }

  result_matrix = identity_matrix(some_matrix.rows);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Warning, memory allocation failure.\n");
    return result_matrix;
  }

  for (iterator = 0; iterator < power; ++iterator) {

    old_matrix = result_matrix;
    result_matrix = matrix_multiplication(result_matrix, some_matrix);
    matrix_free_memory(&old_matrix);

    if (NULL == result_matrix.matrix_ptr) {
      return (matrix) { .matrix_ptr = NULL };
    }
  }

  return result_matrix;
}

matrix matrix_kronecker_product(matrix first, matrix second)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  int row, col, frow, fcol, srow, scol;

  result_matrix = matrix_allocate_memory(first.rows * second.rows, 
					 first.columns * second.columns);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return result_matrix;
  }

  row = col = 0;
  for (frow = 0; frow != first.rows; ++frow) {

    for (fcol = 0; fcol != first.columns; ++fcol) {

      for (scol = 0; scol != second.columns; ++scol, ++col) {

	for (srow = 0; srow != second.rows; ++srow) {

	  result_matrix.matrix_ptr[row + srow][col] = first.matrix_ptr[frow][fcol] * 
	    second.matrix_ptr[srow][scol];
	}
      }
    }
    row += 2;
    col = 0;
  }

  return result_matrix;
}

matrix matrix_hadamard_product(matrix first, matrix second)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  int row, col;

  if ((first.rows != second.rows) || (first.columns != second.columns)) {
    perror("Warning, matrix dimensions do not match.\n");
    return result_matrix;
  }
  else if ((NULL == first.matrix_ptr) || NULL == (second.matrix_ptr)) {
    perror("Warning, undefined operand.\n");
    return result_matrix;
  }

  result_matrix = matrix_allocate_memory(first.rows, first.columns);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Warning, memory allocation failure.\n");
    return result_matrix;
  }

  for (row = 0; row < first.rows; ++row) {

    for (col = 0; col < first.columns; ++col) {

      result_matrix.matrix_ptr[row][col] = first.matrix_ptr[row][col] *
	second.matrix_ptr[row][col];
    }
  }

  return result_matrix;
}

long double matrix_determinant_calculation(matrix some_matrix)
{
  long double det = 0.0L;
  matrix current_matrix;
  int i;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return NAN;
  }

  if (some_matrix.rows == 2) {

    return some_matrix.matrix_ptr[0][0] * some_matrix.matrix_ptr[1][1] -
      some_matrix.matrix_ptr[0][1] * some_matrix.matrix_ptr[1][0];
  }
  else if (some_matrix.rows == 1) {

    return some_matrix.matrix_ptr[0][0];
  }

  for (i = 0; i != some_matrix.rows; ++i) {

    current_matrix = matrix_submatrix(some_matrix, 1, 1 + i);

    if (NULL == current_matrix.matrix_ptr) {
      return NAN;
    }

    det += powl(-1, i) * some_matrix.matrix_ptr[0][i] * 
      matrix_determinant_calculation(current_matrix); 
    matrix_free_memory(&current_matrix);
  }

  return det;
}

long double matrix_trace(matrix some_matrix)
{
  long double trace = 0.0;
  int index;

  if (some_matrix.rows != some_matrix.columns) {
    
    return NAN;
  }

  for (index = 0; index < some_matrix.rows; ++index) {

    trace += some_matrix.matrix_ptr[index][index];
  }

  return trace;
}

long double matrix_frobenius_product(matrix first_matrix, matrix second_matrix)
{
  long double product;
  matrix first_transpose, product_matrix;

  first_transpose = matrix_transposition(first_matrix);

  if (NULL == first_transpose.matrix_ptr) {
    
    return NAN;
  }

  product_matrix = matrix_multiplication(first_transpose, second_matrix);
  
  if (NULL == product_matrix.matrix_ptr) {
    matrix_free_memory(&first_transpose);
    return NAN;
  }

  product = matrix_trace(product_matrix);
  matrix_free_memory(&first_transpose);
  matrix_free_memory(&product_matrix);

  return product;
}

matrix identity_matrix(int rows)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  int index;

  result_matrix = matrix_allocate_memory(rows, rows);

  if (NULL == result_matrix.matrix_ptr) {

    return result_matrix;
  }

  for (index = 0; index < rows; ++index) {

    result_matrix.matrix_ptr[index][index] = 1.0;
  }

  return result_matrix;
}

matrix matrix_kronecker_sum(matrix first_matrix, matrix second_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL }, identityA, identityB;
  matrix termA, termB;

  if ((first_matrix.rows != first_matrix.columns) || 
      (second_matrix.rows != second_matrix.columns)) {
    perror("Warning, not a square matrix.\n");
    return result_matrix;
  }
  else if (first_matrix.rows != second_matrix.rows) {
    perror("Warning, matrix dimensions do not match.\n");
    return result_matrix;
  }

  identityA = identity_matrix(first_matrix.rows);

  if (NULL == identityA.matrix_ptr) {
    return result_matrix;
  }

  identityB = identity_matrix(second_matrix.rows);

  if (NULL == identityB.matrix_ptr) {
    matrix_free_memory(&identityA);
    return result_matrix;
  }

  termA = matrix_kronecker_product(first_matrix, identityA);
  
  if (NULL == termA.matrix_ptr) {
    matrix_free_memory(&identityA);
    matrix_free_memory(&identityB);    
    return result_matrix;
  }

  termB = matrix_kronecker_product(second_matrix, identityB);

  if (NULL == termB.matrix_ptr) {
    matrix_free_memory(&termA);
    matrix_free_memory(&identityA);
    matrix_free_memory(&identityB);
    return result_matrix;
  }

  matrix_self_addition(termA, termB);

  matrix_free_memory(&termB);
  result_matrix = termA;

  matrix_free_memory(&identityA);
  matrix_free_memory(&identityB);

  return result_matrix;
}

matrix matrix_vectorization(matrix some_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  int row, col, vector_index = 0;

  result_matrix = matrix_allocate_memory(some_matrix.rows * 
					 some_matrix.columns, 1);

  if (NULL == result_matrix.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return result_matrix;
  }

  for (col = 0; col < some_matrix.columns; ++col) {

    for (row = 0; row < some_matrix.rows; ++row) {

      result_matrix.matrix_ptr[vector_index++][0] = 
	some_matrix.matrix_ptr[row][col];
    }
  }

  return result_matrix;
}

bool matrix_equality(matrix first_matrix, matrix second_matrix)
{
  bool verdict = true;
  int row, col;

  if ((first_matrix.rows != second_matrix.rows) ||
      (first_matrix.columns != second_matrix.columns)) {

    return false;
  }

  for (row = 0; row < first_matrix.rows; ++row) {

    for (col = 0; col < first_matrix.columns; ++col) {

      if (first_matrix.matrix_ptr[row][col] != 
	  second_matrix.matrix_ptr[row][col]) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

matrix matrix_outer_product(matrix first_vector, matrix second_vector)
{
  matrix result_matrix = { .matrix_ptr = NULL }, vector_transpose;

  if ((first_vector.columns != 1) || (second_vector.columns != 1)) {
    perror("Invalid arguments, expected column vectors.\n");
    return result_matrix;
  }

  vector_transpose = matrix_transposition(second_vector);

  if (NULL == vector_transpose.matrix_ptr) {
    perror("Sorry, matrix transposition failed.\n");
    return result_matrix;
  }

  result_matrix = matrix_multiplication(first_vector, vector_transpose);
  matrix_free_memory(&vector_transpose);

  return result_matrix;
}

long double matrix_frobenius_norm(matrix some_matrix)
{
  matrix conj_tran, mul;
  long double norm = 0.0L;

  conj_tran = matrix_transposition(some_matrix);

  if (NULL == conj_tran.matrix_ptr) {
    return NAN;
  }

  mul = matrix_multiplication(conj_tran, some_matrix);
  matrix_free_memory(&conj_tran);

  if (NULL == mul.matrix_ptr) {
    return NAN;
  }

  norm = matrix_trace(mul);
  matrix_free_memory(&mul);

  return sqrtl(norm);
}

long double matrix_condition_number(matrix some_matrix)
{
  long double max_value, sum;
  int row, col;

  for (row = 0; row != some_matrix.rows; ++row) {

    sum = 0.0L;
    for (col = 0; col != some_matrix.columns; ++col) {

      sum += fabsl(some_matrix.matrix_ptr[row][col]);
    }

    if (!row) {
      max_value = sum;
    }
    else if (max_value < sum) {
      max_value = sum;
    }
  }

  return max_value;
}

matrix eigenvectors_by_simultaneous_iteration(matrix some_matrix)
{
  matrix eigenvectors = { .matrix_ptr = NULL }, vector_basis, current_matrix;
  matrix_pair qr_factor;
  const int iterations = 20;
  int i;

  vector_basis = identity_matrix(some_matrix.rows);

  if (NULL == vector_basis.matrix_ptr) {
    return eigenvectors;
  }

  qr_factor = matrix_qr_decomposition(vector_basis);

  if ((NULL == qr_factor.first.matrix_ptr) || (NULL == qr_factor.second.matrix_ptr)) {
    matrix_free_memory(&vector_basis);
    return eigenvectors;
  }

  for (i = 0; i != iterations; ++i) {

    current_matrix = matrix_multiplication(some_matrix, qr_factor.first);
    matrix_free_memory(&qr_factor.second);
    matrix_free_memory(&qr_factor.first);

    if (NULL == current_matrix.matrix_ptr) {
      matrix_free_memory(&vector_basis);
      return eigenvectors;
    }

    qr_factor = matrix_qr_decomposition(current_matrix);

    if ((NULL == qr_factor.first.matrix_ptr) || (NULL == qr_factor.second.matrix_ptr)) {
      matrix_free_memory(&vector_basis);
      matrix_free_memory(&current_matrix);
      return eigenvectors;
    }
  }

  matrix_free_memory(&qr_factor.second);
  matrix_free_memory(&current_matrix);
  matrix_free_memory(&vector_basis);

  eigenvectors = qr_factor.first;

  return eigenvectors;
}

matrix matrix_eigenvector_power_iteration(matrix some_matrix)
{
  matrix eigenvector = { .matrix_ptr = NULL }, guess_vector = { .matrix_ptr = NULL };
  matrix product;
  int row, i;
  long double norm;
  const int iterations = 10;

  guess_vector = matrix_allocate_memory(some_matrix.rows, 1);

  if (NULL == guess_vector.matrix_ptr) {

    perror("Sorry, memory allocation failed.\n");
    return guess_vector;
  }

  for (row = 0; row != some_matrix.rows; ++row) {

    guess_vector.matrix_ptr[row][0] = 1.0L;
  }

  for (i = 0; i != iterations; ++i) {

    product = matrix_multiplication(some_matrix, guess_vector);
    matrix_free_memory(&guess_vector);

    if (NULL == product.matrix_ptr) {
      return (matrix) { .matrix_ptr = NULL };
    }

    guess_vector = product;
  }

  norm = vector_norm(guess_vector);

  if (0.0L == norm) {

    perror("Sorry, can't find the eigenvector. Division by zero.\n");
    matrix_free_memory(&guess_vector);
    return eigenvector;
  }

  scalar_matrix_multiplication(guess_vector, 1 / norm);
  eigenvector = guess_vector;

  return eigenvector;
}

matrix qr_iteration_for_eigenvalues(matrix some_matrix)
{
  matrix rq_product, eigenvalue_matrix = { .matrix_ptr = NULL };
  matrix_pair qr;
  const int iterations = 20;
  int it, col;

  if ((NULL == some_matrix.matrix_ptr) || (some_matrix.rows != some_matrix.columns)) {
    perror("Warning, invalid parameter.\n");
    return eigenvalue_matrix;
  }

  rq_product = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == rq_product.matrix_ptr) {
    return rq_product;
  }

  for (it = 0; it != iterations; ++it) {

    qr = matrix_qr_by_givens_rotation(rq_product);
    matrix_free_memory(&rq_product);

    if ((NULL == qr.first.matrix_ptr) || (NULL == qr.second.matrix_ptr)) {
      return eigenvalue_matrix;
    }

    rq_product = matrix_multiplication(qr.second, qr.first);
    matrix_free_memory(&qr.first);
    matrix_free_memory(&qr.second);

    if (NULL == rq_product.matrix_ptr) {
      return (matrix) { .matrix_ptr = NULL };
    }
  }
  
  eigenvalue_matrix = matrix_allocate_memory(1, rq_product.columns);

  if (NULL == eigenvalue_matrix.matrix_ptr) {
    matrix_free_memory(&rq_product);
    perror("Sorry, memory allocation failure.\n");
    return eigenvalue_matrix;
  }

  for (col = 0; col != rq_product.columns; ++col) {

    eigenvalue_matrix.matrix_ptr[0][col] = rq_product.matrix_ptr[col][col];
  }

  matrix_free_memory(&rq_product);

  return eigenvalue_matrix;
}

int matrix_get_rank(matrix some_matrix)
{
  matrix eigenvalues, transpose, multiplication;
  int rank = 0, i;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return -1;
  }
  
  transpose = matrix_transposition(some_matrix);

  if (NULL == some_matrix.matrix_ptr) {
    perror("Sorry, matrix transpose error.\n");
    return -1;
  }

  multiplication = matrix_multiplication(some_matrix, transpose);
  matrix_free_memory(&transpose);

  if (NULL == multiplication.matrix_ptr) {
    perror("Sorry, matrix multiplication error.\n");
    return -1;
  }

  eigenvalues = qr_iteration_for_eigenvalues(multiplication);
  matrix_free_memory(&multiplication);

  if (NULL == eigenvalues.matrix_ptr) {
    perror("Sorry, eigenvalue calculation error.\n");
    return -1;
  }
  
  for (i = 0; i != eigenvalues.columns; ++i) {

    if ((fabsl(eigenvalues.matrix_ptr[0][i]) - 0.0000001) > 0.00000001) {
      ++rank;
    }
  }

  matrix_free_memory(&eigenvalues);

  return rank;
}

long double factorial(unsigned int n)
{
  if (!n) {
    return 1.0L;
  }
  return n * factorial(n - 1);
}

matrix matrix_square_root(matrix some_matrix)
{
  matrix sqroot = { .matrix_ptr = NULL }, current_matrix, inverse, mul_result, sqr;
  const int iterations = 20;
  int i, row, col;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return sqroot;
  }
  else if (!is_invertible_matrix(some_matrix)) {
    perror("Sorry, the matrix isn't invertible.\n");
    return sqroot;
  }

  current_matrix = identity_matrix(some_matrix.rows);

  if (NULL == current_matrix.matrix_ptr) {
    return current_matrix;
  }

  for (i = 0; i != iterations; ++i) {

    if (!is_invertible_matrix(current_matrix)) {
      perror("Sorry, the matrix isn't invertible.\n");
      return sqroot;
    }

    inverse = matrix_inverse(current_matrix);

    if (NULL == inverse.matrix_ptr) {
      perror("Sorry, matrix inversion failed.\n");
      matrix_free_memory(&current_matrix);
      return (matrix) { .matrix_ptr = NULL };
    }

    mul_result = matrix_multiplication(some_matrix, inverse);

    if (NULL == mul_result.matrix_ptr) {
      matrix_free_memory(&inverse);
      matrix_free_memory(&current_matrix);      
      return mul_result;
    }

    matrix_self_addition(current_matrix, mul_result);
    scalar_matrix_multiplication(current_matrix, 1.0L / 2);
    matrix_free_memory(&mul_result);
    matrix_free_memory(&inverse);
  }

  sqroot = current_matrix;
  sqr = matrix_power(sqroot, 2);

  if (NULL != sqr.matrix_ptr) {
   
    for (row = 0; (row != sqr.rows) && (sqroot.matrix_ptr != NULL); ++row) {

      for (col = 0; col != sqr.columns; ++col) {

	if (fabsl(sqr.matrix_ptr[row][col] - some_matrix.matrix_ptr[row][col]) > 0.7L) {

	  matrix_free_memory(&sqroot);
	  break;
	}
      }
    }

    matrix_free_memory(&sqr);
  }
  else {

    matrix_free_memory(&sqroot);
  }

  return sqroot;
}

matrix matrix_sine(matrix some_matrix)
{
  matrix matrix_result = { .matrix_ptr = NULL }, matrix_exp;
  const int iterations = 30;
  int i;

  matrix_result = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == matrix_result.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return matrix_result;
  }

  for (i = 1; i != iterations; ++i) {

    matrix_exp = matrix_power(some_matrix, 2 * i + 1);

    if (NULL == matrix_exp.matrix_ptr) {
      matrix_free_memory(&matrix_result);
      return matrix_exp;
    }

    scalar_matrix_multiplication(matrix_exp, powl(-1.0L, i) / factorial(2 * i + 1));
    matrix_self_addition(matrix_result, matrix_exp);
    matrix_free_memory(&matrix_exp);
  }

  return matrix_result;
}

matrix matrix_cosine(matrix some_matrix)
{
  matrix matrix_result = { .matrix_ptr = NULL }, matrix_exp;
  const int iterations = 30;
  int i;

  matrix_result = identity_matrix(some_matrix.rows);

  if (NULL == matrix_result.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return matrix_result;
  }

  for (i = 1; i != iterations; ++i) {

    matrix_exp = matrix_power(some_matrix, 2 * i);

    if (NULL == matrix_exp.matrix_ptr) {
      perror("Sorry, memory allocation failed.\n");
      return matrix_exp;
    }

    scalar_matrix_multiplication(matrix_exp, powl(-1.0L, i) / factorial(2 * i));
    matrix_self_addition(matrix_result, matrix_exp);
    matrix_free_memory(&matrix_exp);
  }

  return matrix_result;
}

matrix matrix_sign_function(matrix some_matrix)
{
  matrix sign = { .matrix_ptr = NULL }, current_matrix, inverse;
  const int iterations = 25;
  int i;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return sign;
  }

  current_matrix = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == current_matrix.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return current_matrix;
  }

  for (i = 0; i != iterations; ++i) {

    inverse = matrix_inverse(current_matrix);
    
    if (NULL == inverse.matrix_ptr) {
      perror("Sorry, matrix inversion failed.\n");
      matrix_free_memory(&current_matrix);
      return inverse;
    }

    matrix_self_addition(current_matrix, inverse);
    matrix_free_memory(&inverse);
    scalar_matrix_multiplication(current_matrix, 1.0L / 2);
  }

  sign = current_matrix;

  return sign;
}

matrix identity_matrix_NxM(int total_rows, int total_columns)
{
  matrix identity = { .matrix_ptr = NULL };
  int i;

  identity = matrix_allocate_memory(total_rows, total_columns);

  if (NULL == identity.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return identity;
  }

  for (i = 0; i != total_columns; ++i) {

    identity.matrix_ptr[i][i] = 1.0L;
  }

  return identity;
}

matrix matrix_exponential(matrix some_matrix)
{
  matrix exp_matrix = { .matrix_ptr = NULL }, current_matrix, prev_matrix;
  const int iterations = 20;
  int i, t = 1;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return exp_matrix;
  }

  exp_matrix = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == exp_matrix.matrix_ptr) {
    return exp_matrix;
  }

  current_matrix = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == current_matrix.matrix_ptr) {
    matrix_free_memory(&exp_matrix);
    return exp_matrix;
  }

  for (i = 0; i != iterations; ++i) {

    prev_matrix = current_matrix;
    current_matrix = matrix_power(some_matrix, i);

    if (NULL == current_matrix.matrix_ptr) {
      matrix_free_memory(&exp_matrix);
      return exp_matrix;
    }
    
    scalar_matrix_multiplication(current_matrix, powl(t, i) / factorial(i));
    matrix_free_memory(&prev_matrix);
    matrix_self_addition(exp_matrix, current_matrix);
  }

  matrix_free_memory(&current_matrix);

  return exp_matrix;
}

matrix solve_system_using_lup_decompozition(matrix coefficients_matrix, matrix right_matrix)
{
  matrix solution = { .matrix_ptr = NULL }, product, intermediate;
  matrix_tuple lup;
  int i, j, row;
  long double sum;

  intermediate = matrix_allocate_memory(right_matrix.rows, right_matrix.columns);

  if (NULL == intermediate.matrix_ptr) {
    return solution;
  }

  lup = matrix_lup_decomposition(coefficients_matrix);

  if ((NULL == lup.first.matrix_ptr) || (NULL == lup.second.matrix_ptr) || 
      (NULL == lup.third.matrix_ptr)) {
    matrix_free_memory(&intermediate);
    return solution;
  }

  product = matrix_multiplication(lup.third, right_matrix);

  if (NULL == product.matrix_ptr) {
    matrix_free_memory(&lup.first);
    matrix_free_memory(&lup.second);
    matrix_free_memory(&lup.third);
    matrix_free_memory(&intermediate);
    return solution;
  }

  for (i = 0; i != right_matrix.rows; ++i) {

    sum = 0.0L;
    for (j = 0; j <= (i - 1); ++j) {

      sum += lup.first.matrix_ptr[i][j] * intermediate.matrix_ptr[j][0];
    }

    if (fabsl(lup.first.matrix_ptr[i][i]) - 0.00000000L < 0.000000000L) {
      matrix_free_memory(&lup.first);
      matrix_free_memory(&lup.second);
      matrix_free_memory(&lup.third);
      matrix_free_memory(&intermediate);
      matrix_free_memory(&product);
      return solution;
    }

    intermediate.matrix_ptr[i][0] = (product.matrix_ptr[i][0] - sum) / lup.first.matrix_ptr[i][i];
  }

  solution = matrix_allocate_memory(right_matrix.rows, 1);

  if (NULL == solution.matrix_ptr) {
      matrix_free_memory(&lup.first);
      matrix_free_memory(&lup.second);
      matrix_free_memory(&lup.third);
      matrix_free_memory(&intermediate);
      matrix_free_memory(&product);
    return solution;
  }

  for (row = (right_matrix.rows - 1); row >= 0; --row) {

    sum = 0.0L;
    for (i = row + 1; i < right_matrix.rows; ++i) {

      sum += solution.matrix_ptr[i][0] * lup.second.matrix_ptr[row][i];
    }

    if (fabsl(lup.second.matrix_ptr[row][row]) - 0.00000000L < 0.000000000L) {
      matrix_free_memory(&lup.first);
      matrix_free_memory(&lup.second);
      matrix_free_memory(&lup.third);
      matrix_free_memory(&intermediate);
      matrix_free_memory(&product);
      matrix_free_memory(&solution);
      return solution;
    }

    solution.matrix_ptr[row][0] = (intermediate.matrix_ptr[row][0] - sum) 
      / lup.second.matrix_ptr[row][row];
  }

  matrix_free_memory(&lup.first);
  matrix_free_memory(&lup.second);
  matrix_free_memory(&lup.third);
  matrix_free_memory(&intermediate);
  matrix_free_memory(&product);

  return solution;
}

matrix solve_system_using_qr_decomposition(matrix coefficients_matrix, matrix right_matrix)
{
  matrix solution = { .matrix_ptr = NULL }, result_vector, qtranspose;
  matrix_pair qr;
  long double sum = 0.0L;
  int row, i;

  solution = matrix_allocate_memory(right_matrix.rows, 1);

  if (NULL == solution.matrix_ptr) {
    perror("Warning, memory allocation failed.\n");
    return solution;
  }

  qr = matrix_qr_decomposition(coefficients_matrix);

  if ((NULL == qr.first.matrix_ptr) || (NULL == qr.second.matrix_ptr)) {
    matrix_free_memory(&solution);
    return solution;
  }

  qtranspose = matrix_transposition(qr.first);

  if (NULL == qtranspose.matrix_ptr) {
    matrix_free_memory(&qr.first);
    matrix_free_memory(&qr.second);
    matrix_free_memory(&solution);
    return solution;
  }

  result_vector = matrix_multiplication(qtranspose, right_matrix);

  if (NULL == result_vector.matrix_ptr) {
    matrix_free_memory(&qr.first);
    matrix_free_memory(&qr.second);
    matrix_free_memory(&solution);
    matrix_free_memory(&qtranspose);
    return solution;
  }
  
  for (row = (right_matrix.rows - 1); row >= 0; --row) {

    sum = 0.0L;
    for (i = row + 1; i < right_matrix.rows; ++i) {

      sum += solution.matrix_ptr[i][0] * qr.second.matrix_ptr[row][i];
    }

    if (fabsl(qr.second.matrix_ptr[row][row]) - 0.00000000L < 0.000000000L) {
      matrix_free_memory(&qr.first);
      matrix_free_memory(&qr.second);
      matrix_free_memory(&qtranspose);
      matrix_free_memory(&result_vector);
      matrix_free_memory(&solution);
      return solution;
    }

    solution.matrix_ptr[row][0] = (result_vector.matrix_ptr[row][0] - sum) 
      / qr.second.matrix_ptr[row][row];
  }

  matrix_free_memory(&qr.first);
  matrix_free_memory(&qr.second);
  matrix_free_memory(&qtranspose);
  matrix_free_memory(&result_vector);

  return solution;
}

matrix qr_matrix_eigenvectors(matrix some_matrix)
{
  matrix prev_matrix, rq_product, eigenvector_matrix = { .matrix_ptr = NULL };
  matrix qproduct;
  matrix_pair qr;
  const int iterations = 40;
  int it, col;

  if ((NULL == some_matrix.matrix_ptr) || (some_matrix.rows != some_matrix.columns)) {
    perror("Warning, invalid parameter.\n");
    return eigenvector_matrix;
  }

  rq_product = matrix_scalar_multiplication(some_matrix, 1.0L);
  prev_matrix = identity_matrix(some_matrix.rows);

  for (it = 0; it != iterations; ++it) {

    qr = matrix_qr_by_givens_rotation(rq_product);
    matrix_free_memory(&rq_product);

    qproduct = matrix_multiplication(prev_matrix, qr.first);
    matrix_free_memory(&prev_matrix);
    prev_matrix = qproduct;

    rq_product = matrix_multiplication(qr.second, qr.first);
    matrix_free_memory(&qr.first);
    matrix_free_memory(&qr.second);

    if (NULL == rq_product.matrix_ptr) {
      return (matrix) { .matrix_ptr = NULL };
    }
  }

  eigenvector_matrix = matrix_allocate_memory(qproduct.columns, qproduct.rows);
  
  for (col = 0; col != qproduct.columns; ++col) {

    for (int row = 0; row != qproduct.rows; ++row) {

      eigenvector_matrix.matrix_ptr[col][row] = qproduct.matrix_ptr[row][col];
    }
  }

  matrix_free_memory(&rq_product);

  return eigenvector_matrix;
}

matrix_tuple matrix_lup_decomposition(matrix some_matrix)
{
  matrix_tuple lup;
  matrix current_matrix, prev_matrix;
  long double matrix_item;
  int i, j, index, row, exchanges = 0;

  if (NULL == some_matrix.matrix_ptr) {
    return lup;
  }
  else if (some_matrix.rows != some_matrix.columns) {
    return lup;
  }

  lup.first = identity_matrix(some_matrix.rows);
  lup.third = matrix_allocate_memory(some_matrix.rows, 1);

  if (NULL == lup.first.matrix_ptr) {
    matrix_free_memory(&lup.second);
    return lup;
  }
  else if (NULL == lup.third.matrix_ptr) {
    matrix_free_memory(&lup.first);
    matrix_free_memory(&lup.second);
    return lup;
  }

  current_matrix = matrix_scalar_multiplication(some_matrix, 1.0L);

  for (i = 0; i != some_matrix.rows; ++i) {

    lup.first.matrix_ptr[i][i] = 1.0L;
    lup.third.matrix_ptr[i][0] = i;
  }

  for (row = 0; row != some_matrix.rows; ++row) {

    matrix_item = 0.0L;
    index = 0;
    for (i = row; i != some_matrix.rows; ++i) {

      if (fabsl(current_matrix.matrix_ptr[i][row]) > fabsl(matrix_item)) {

	matrix_item = current_matrix.matrix_ptr[i][row];
	index = i;
      }
    }

    if (0.0L == matrix_item) {
      //      perror("Sorry, the matrix is singular.\n");
      matrix_free_memory(&lup.first);
      matrix_free_memory(&lup.third);
      matrix_free_memory(&current_matrix);
      return lup;
    }

    matrix_item = lup.third.matrix_ptr[row][0];
    lup.third.matrix_ptr[row][0] = lup.third.matrix_ptr[index][0];
    lup.third.matrix_ptr[index][0] = matrix_item;

    for (i = 0; i != current_matrix.rows; ++i) {

      if (!i) {
	++exchanges; 
      }

      matrix_item = current_matrix.matrix_ptr[row][i];
      current_matrix.matrix_ptr[row][i] = current_matrix.matrix_ptr[index][i];
      current_matrix.matrix_ptr[index][i] = matrix_item;
    }

    for (i = row + 1; i != current_matrix.rows; ++i) {

      current_matrix.matrix_ptr[i][row] /= current_matrix.matrix_ptr[row][row];

      for (j = row + 1; j != current_matrix.rows; ++j) {

	current_matrix.matrix_ptr[i][j] = current_matrix.matrix_ptr[i][j] - 
	  current_matrix.matrix_ptr[i][row] * current_matrix.matrix_ptr[row][j];
      }
    }
  }

  prev_matrix = lup.third;
  lup.third = matrix_allocate_memory(current_matrix.rows, current_matrix.rows);

  for (i = 0; i != current_matrix.rows; ++i) {

    index = (int) prev_matrix.matrix_ptr[i][0];
    lup.third.matrix_ptr[i][index] = 1.0L;
    lup.first.matrix_ptr[i][i] = 1.0L;

    for (j = i + 1; j != current_matrix.rows; ++j) {

      lup.first.matrix_ptr[j][i] = current_matrix.matrix_ptr[j][i];
      current_matrix.matrix_ptr[j][i] = 0.0L;
    }
  }

  matrix_free_memory(&prev_matrix);
  lup.second = current_matrix;


  lup.row_exchanges = --exchanges;

  return lup;
}

matrix_pair matrix_lu_decomposition(matrix some_matrix)
{
  matrix_pair lu = { .first.matrix_ptr = NULL, .second.matrix_ptr = NULL};
  matrix current_matrix, prev_matrix, row_vector, col_vector, schur_matrix;
  int i, index;

  if (NULL == some_matrix.matrix_ptr) {
    return lu;
  }
  else if (some_matrix.rows != some_matrix.columns) {
    return lu;
  }

  lu.first = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);
  lu.second = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == lu.first.matrix_ptr) {
    matrix_free_memory(&lu.second);
    return lu;
  }
  else if (NULL == lu.second.matrix_ptr) {
    matrix_free_memory(&lu.first);
    return lu;
  }

  current_matrix = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == current_matrix.matrix_ptr) {
    matrix_free_memory(&lu.first);
    matrix_free_memory(&lu.second);
    return lu;
  }

  for (i = 0; i != some_matrix.rows; ++i) {

    lu.first.matrix_ptr[i][i] = 1.0L;
  }

  for (i = 0; i != some_matrix.rows; ++i) {

    row_vector = matrix_allocate_memory(1, current_matrix.columns - 1);
    col_vector = matrix_allocate_memory(current_matrix.rows - 1, 1);

    for (index = 0; index != current_matrix.rows; ++index) {

      if (index) {
	
	if (0.0L == current_matrix.matrix_ptr[0][0]) {
	  matrix_free_memory(&row_vector);
	  matrix_free_memory(&col_vector);
	  matrix_free_memory(&lu.first);
	  matrix_free_memory(&lu.second);
	  perror("Warning, cannot perform the LU decomposition (division by zero).\n");
	  return (matrix_pair) { .first.matrix_ptr = NULL, .second.matrix_ptr = NULL };
	}
	else {
	  lu.first.matrix_ptr[index + i][i] = current_matrix.matrix_ptr[index][0] / 
	    current_matrix.matrix_ptr[0][0];
	}

	row_vector.matrix_ptr[0][index - 1] = current_matrix.matrix_ptr[0][index];
	col_vector.matrix_ptr[index - 1][0] = lu.first.matrix_ptr[index + i][i];
      }

      lu.second.matrix_ptr[i][index + i] = current_matrix.matrix_ptr[0][index];
    }

    schur_matrix = matrix_multiplication(col_vector, row_vector);
    scalar_matrix_multiplication(schur_matrix, -1.0L);

    prev_matrix = current_matrix;
    current_matrix = matrix_submatrix(current_matrix, 1, 1);
    matrix_self_addition(schur_matrix, current_matrix);
    matrix_free_memory(&prev_matrix);

    prev_matrix = current_matrix;
    current_matrix = schur_matrix;
    matrix_free_memory(&prev_matrix);
    matrix_free_memory(&row_vector);
    matrix_free_memory(&col_vector);
  }

  return lu;
}

long double vector_norm(matrix some_vector)
{
  long double sum = 0.0L;
  int row, col;

  if (1 == some_vector.rows) {

    row = 0;
    for (col = 0; col != some_vector.columns; ++col) {

      sum += powl(some_vector.matrix_ptr[row][col], 2);
    }
  }
  else {

    col = 0;
    for (row = 0; row != some_vector.rows; ++row) {

      sum += powl(some_vector.matrix_ptr[row][col], 2);
    }
  }

  return sqrtl(sum); 
}

long double matrix_inner_product(matrix first_vector, matrix second_vector)
{
  long double product = 0.0L;
  int row;

  if ((first_vector.columns != 1) || (second_vector.columns != 1)) {
    perror("Warning, unexpected matrix dimensions.\n");
    return NAN;
  }
  else if (first_vector.rows != second_vector.rows) {
    perror("Warning, vector dimensions do not match.\n");
    return NAN;
  }

  for (row = 0; row != first_vector.rows; ++row) {

    product += first_vector.matrix_ptr[row][0] * second_vector.matrix_ptr[row][0];
  }

  return product;
}

/*
matrix_pair gram_schmidt_qr_decomposition(matrix some_matrix)
{
  matrix_pair qr = { .first.matrix_ptr = NULL, .second.matrix_ptr = NULL };
  matrix current_vector, *unit_vectors, transpose;
  const int num_of_vectors = some_matrix.columns;
  long double norm, proj;
  int row, col, vector_index, i;

  qr.first = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == qr.first.matrix_ptr) {
    perror("Sorry, memory allocation failed.\n");
    return qr;
  }
  
  unit_vectors = (matrix*) malloc(num_of_vectors * sizeof(*unit_vectors));

  if (NULL == unit_vectors) {
    matrix_free_memory(&qr.first);
    perror("Sorry, memory allocation failed.\n");
    return qr;
  }

  current_vector = matrix_allocate_memory(some_matrix.rows, 1);

  if (NULL == current_vector.matrix_ptr) {
    matrix_free_memory(&qr.first);
    matrix_free_memory(&qr.second);
    perror("Sorry, memory allocation failed.\n");
    return qr;
  }

  vector_index = 0;
  for (col = 0; col != some_matrix.columns; ++col) {
    
    unit_vectors[vector_index] = matrix_allocate_memory(some_matrix.rows, 1);

    if (NULL == unit_vectors[vector_index].matrix_ptr) {
      //...
    }

    for (row = 0; row != some_matrix.rows; ++row) {

      current_vector.matrix_ptr[row][0] = some_matrix.matrix_ptr[row][col];
    }

    for (i = 0; i < col; ++i) {

      proj = matrix_inner_product(unit_vectors[i], current_vector);
      proj /= matrix_inner_product(unit_vectors[i], unit_vectors[i]);
      scalar_matrix_multiplication(unit_vectors[i], -proj);
      matrix_self_addition(current_vector, unit_vectors[i]);
      scalar_matrix_multiplication(unit_vectors[i], -1.0L / proj);
    }

    for (i = 0; i != some_matrix.rows; ++i) {

      unit_vectors[vector_index].matrix_ptr[i][0] = current_vector.matrix_ptr[i][0];
    }

    norm = vector_norm(current_vector);
    scalar_matrix_multiplication(unit_vectors[vector_index], 1.0L / norm);

    for (i = 0; i != qr.first.rows; ++i) {

      qr.first.matrix_ptr[i][col] = unit_vectors[vector_index].matrix_ptr[i][0];
    }
    ++vector_index;
  }

  for (i = 0; i != num_of_vectors; ++i) {

    matrix_free_memory(&unit_vectors[i]);
  }

  free(unit_vectors);
  matrix_free_memory(&current_vector);

  transpose = matrix_transposition(qr.first);
  qr.second = matrix_multiplication(transpose, some_matrix);
  matrix_free_memory(&transpose);

  return qr;
}
*/

matrix_pair matrix_qr_decomposition(matrix some_matrix)
{
  matrix_pair qr = { .first.matrix_ptr = NULL, .second.matrix_ptr = NULL };
  int col, vector_col, householder_index = 0, irow, icol, row_diff, col_diff;
  matrix *householder_matrices, householder_vector, column_vector, matrix_product;
  matrix householder_transpose, identity, current_matrix, prev_matrix;
  long double norm;
  const int num_of_vectors = (some_matrix.columns >= some_matrix.rows) ? some_matrix.columns - 1
    : some_matrix.columns;

  householder_matrices = (matrix *) calloc(num_of_vectors,
					   sizeof(*householder_matrices));
  if (NULL == householder_matrices) {
    perror("Warning, memory allocation failed.\n");
    return qr;
  }

  current_matrix = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == current_matrix.matrix_ptr) {
    free(householder_matrices);
    return qr;
  }

  for (col = 0; col != num_of_vectors; ++col, ++householder_index) {

    column_vector = matrix_allocate_memory(current_matrix.rows - col, 1);

    if (NULL == column_vector.matrix_ptr) {
      matrix_free_memory(&current_matrix);
      free(householder_matrices);
      return qr;
    }

    for (vector_col = col; vector_col != current_matrix.rows; ++vector_col) {

      column_vector.matrix_ptr[vector_col - col][0] = 
	current_matrix.matrix_ptr[vector_col][col];
    }

    norm = vector_norm(column_vector);
    householder_vector = column_vector;
    column_vector.matrix_ptr = NULL;
    householder_vector.matrix_ptr[0][0] += norm;

    norm = vector_norm(householder_vector);

    if (0.0L == norm) {
      perror("Sorry, cannot perform the QR decomposition. Division by zero.\n");
      matrix_free_memory(&householder_vector);
      matrix_free_memory(&current_matrix);
      free(householder_matrices);
      return qr;
    }

    householder_transpose = matrix_transposition(householder_vector);
    householder_matrices[householder_index] = matrix_multiplication(householder_vector,
								    householder_transpose);
    matrix_free_memory(&householder_vector);
    matrix_free_memory(&householder_transpose); 
    scalar_matrix_multiplication(householder_matrices[householder_index], 2 / (norm * norm));

    identity = identity_matrix(householder_matrices[householder_index].rows);
    scalar_matrix_multiplication(householder_matrices[householder_index], -1.0L);
    matrix_self_addition(identity, householder_matrices[householder_index]);
    matrix_free_memory(&householder_matrices[householder_index]);
    householder_matrices[householder_index] = identity;

    identity = identity_matrix(current_matrix.rows);
    row_diff = identity.rows - householder_matrices[householder_index].rows;
    col_diff = identity.columns - householder_matrices[householder_index].columns;

    for (irow = 0; irow != householder_matrices[householder_index].rows; ++irow) {

      for (icol = 0; icol != householder_matrices[householder_index].columns; ++icol) {

	identity.matrix_ptr[irow + row_diff][icol + col_diff] = 
	  householder_matrices[householder_index].matrix_ptr[irow][icol];
      }
    }

    matrix_free_memory(&householder_matrices[householder_index]);
    householder_matrices[householder_index] = identity;
    
    prev_matrix = current_matrix;
    current_matrix = matrix_multiplication(householder_matrices[householder_index], 
					   current_matrix);
    matrix_free_memory(&prev_matrix);
  }

  identity = identity_matrix(householder_matrices[0].rows);
  matrix_product = identity;
  for (householder_index = 0; householder_index != num_of_vectors;
       ++householder_index) {

    prev_matrix = matrix_product;
    matrix_product = matrix_multiplication(matrix_product, 
					   householder_matrices[householder_index]);
    matrix_free_memory(&householder_matrices[householder_index]);
    matrix_free_memory(&prev_matrix);
  }

  qr.second = current_matrix;
  qr.first = matrix_product;
  free(householder_matrices);

  return qr;
}

long double matrix_lu_determinant(matrix some_matrix)
{
  long double determinant = 1.0L;
  matrix_pair lu;
  int i;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return NAN;
  }

  lu = matrix_lu_decomposition(some_matrix);

  if ((NULL == lu.first.matrix_ptr) || (NULL == lu.second.matrix_ptr)) {
    return NAN;
  }

  for (i = 0; i != some_matrix.rows; ++i) {

    //    determinant *= lu.first.matrix_ptr[i][i];
    determinant *= lu.second.matrix_ptr[i][i];
  }

  matrix_free_memory(&lu.first);
  matrix_free_memory(&lu.second);

  return determinant;
}

long double matrix_lup_determinant(matrix some_matrix)
{
  long double determinant = 1.0L;
  matrix_tuple lup;
  int i;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return NAN;
  }

  lup = matrix_lup_decomposition(some_matrix);

  if ((NULL == lup.first.matrix_ptr) || (NULL == lup.second.matrix_ptr) || (NULL == lup.third.matrix_ptr)) {
    return NAN;
  }

  for (i = 0; i != some_matrix.rows; ++i) {

    determinant *= lup.second.matrix_ptr[i][i];
  }

  determinant *= powl(-1.0L, lup.row_exchanges);

  matrix_free_memory(&lup.first);
  matrix_free_memory(&lup.second);
  matrix_free_memory(&lup.third);

  return determinant;
}

matrix matrix_inverse(matrix some_matrix)
{
  matrix inverse = { .matrix_ptr = NULL };
  long double determinant;

  if (NULL == some_matrix.matrix_ptr) {
    perror("Warning, invalid argument.\n");
    return inverse;
  }
  else if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return inverse;
  }

  determinant = matrix_determinant_calculation(some_matrix);

  if ((0.0L == determinant) || isnan(determinant)) {
    perror("Warning, the matrix does not have an inverse.\n");
    return inverse;
  }

  inverse = matrix_adjugate(some_matrix);
  
  if (NULL == inverse.matrix_ptr) {
    return inverse;
  }

  scalar_matrix_multiplication(inverse, 1.0L / determinant);

  return inverse;
}

long double matrix_determinant_using_qr(matrix some_matrix)
{
  long double determinant = 1.0L;
  matrix_pair qr;
  int i;

  if (some_matrix.rows != some_matrix.columns) {
    perror("Warning, not a square matrix.\n");
    return NAN;
  }

  qr = matrix_qr_decomposition(some_matrix);
  
  if ((NULL == qr.first.matrix_ptr) || (NULL == qr.second.matrix_ptr)) {
    return NAN;  
  }

  for (i = 0; i != some_matrix.rows; ++i) {

    determinant *= qr.second.matrix_ptr[i][i];
  }

  matrix_free_memory(&qr.first);
  matrix_free_memory(&qr.second);

  i = (some_matrix.rows <= some_matrix.columns) ? some_matrix.columns - 1: 
    some_matrix.columns;

  determinant *= powl(-1.0L, i);

  return determinant;
}

matrix_pair matrix_qr_by_givens_rotation(matrix some_matrix)
{
  matrix_pair qr = { .first.matrix_ptr = NULL, .second.matrix_ptr = NULL };
  matrix *intermediate, identity, current_matrix, Qmatrix, transpose, prev_matrix;
  long double cosine, sine;
  int row, col, num_of_matrices = 0, i = 0;

  for (row = 0; row != some_matrix.rows; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if (col < row) {
	++num_of_matrices;
      }
    }
  }

  current_matrix = matrix_scalar_multiplication(some_matrix, 1.0L);

  if (NULL == current_matrix.matrix_ptr) {
    return qr;
  }

  intermediate = (matrix *) calloc(num_of_matrices, sizeof(*intermediate));

  if (NULL == intermediate) {
    perror("Sorry, memory allocation failed.\n");
    matrix_free_memory(&current_matrix);
    return qr;
  }

  for (col = 0; col != (some_matrix.columns - 1); ++col) {

    for (row = 0; row != some_matrix.rows; ++row) {

      if (col >= row) {
	continue;
      }

      if ((0.0L == current_matrix.matrix_ptr[col][col])  || (0.0L == current_matrix.matrix_ptr[row][col])) {
	perror("Sorry, cannot perform the QR decomposition. Division by zero.\n");
	matrix_free_memory(&current_matrix);

	for (i = 0; intermediate[i].matrix_ptr; ++i) {
	  
	  matrix_free_memory(&intermediate[i]);
	}

	free(intermediate);
	return qr;
      }

      cosine = current_matrix.matrix_ptr[col][col] / 
	sqrtl(powl(current_matrix.matrix_ptr[col][col], 2) + powl(current_matrix.matrix_ptr[row][col], 2));

      sine = -current_matrix.matrix_ptr[row][col] / 
	sqrtl(powl(current_matrix.matrix_ptr[col][col], 2) + powl(current_matrix.matrix_ptr[row][col], 2));

      identity = identity_matrix(current_matrix.rows);

      if (NULL == identity.matrix_ptr) {
	perror("Sorry, cannot perform the QR decomposition.\n");
	matrix_free_memory(&current_matrix);

	for (i = 0; intermediate[i].matrix_ptr; ++i) {
	  
	  matrix_free_memory(&intermediate[i]);
	}

	free(intermediate);
	return qr;
      }

      identity.matrix_ptr[row][col] = sine;
      identity.matrix_ptr[col][row] = -sine;
      identity.matrix_ptr[col][col] = cosine;
      identity.matrix_ptr[row][row] = cosine;

      prev_matrix = current_matrix;
      current_matrix = matrix_multiplication(identity, current_matrix);
      intermediate[i] = identity;
      matrix_free_memory(&prev_matrix);
      ++i;

      if (NULL == current_matrix.matrix_ptr) {
	perror("Sorry, cannot perform the QR decomposition. Memory allocation failed.\n");

	for (i = 0; intermediate[i].matrix_ptr; ++i) {
	  
	  matrix_free_memory(&intermediate[i]);
	}

	free(intermediate);
	return qr;
      }

    }
  }

  qr.second = current_matrix;
  Qmatrix = identity_matrix(intermediate[0].rows);

  if (NULL == Qmatrix.matrix_ptr) {
    perror("Sorry, cannot perform the QR decomposition.\n");

    for (i = 0; intermediate[i].matrix_ptr; ++i) {
	  
      matrix_free_memory(&intermediate[i]);
    }

    free(intermediate);
    return qr;
  }

  for (i = 0; i != num_of_matrices; ++i) {

    transpose = matrix_transposition(intermediate[i]);
    prev_matrix = Qmatrix;
    Qmatrix = matrix_multiplication(Qmatrix, transpose);
    matrix_free_memory(&transpose);
    matrix_free_memory(&prev_matrix);
    matrix_free_memory(&intermediate[i]);
  }

  free(intermediate);
  qr.first = Qmatrix;

  return qr;
}

matrix matrix_submatrix(matrix some_matrix, int some_row, int some_column)
{
  matrix submatrix = {.matrix_ptr = NULL};
  int submatrix_rows, submatrix_cols, row, col;

  submatrix_rows = (some_row != -1) ? some_matrix.rows - 1 : some_matrix.rows;
  submatrix_cols = (some_column != -1) ? some_matrix.columns - 1 : some_matrix.columns;

  submatrix = matrix_allocate_memory(submatrix_rows, submatrix_cols);

  if (NULL == submatrix.matrix_ptr) {
    perror("Warning, memory allocation submatrix failed.\n");
    return submatrix;
  }

  submatrix_rows = submatrix_cols = 0;
  for (row = 0; row != some_matrix.rows; ++row) {

    if ((row + 1) == some_row) {
      continue;
    }

    for (col = 0; col != some_matrix.columns; ++col) {

      if ((col + 1) == some_column) {
	continue;
      }

      submatrix.matrix_ptr[submatrix_rows][submatrix_cols++] = 
	some_matrix.matrix_ptr[row][col];
    }

    ++submatrix_rows;
    submatrix_cols = 0;
  }

  return submatrix;
}

matrix matrix_cholesky_decomposition(matrix some_matrix)
{
  matrix result_matrix = { .matrix_ptr = NULL };
  long double sum;
  int row, col, i;

  if (some_matrix.rows != some_matrix.columns) {
    return result_matrix;
  }

  if (!is_symmetric_matrix(some_matrix))  {
    perror("Warning, not a symmetric matrix.\n");
    return result_matrix;
  }
  else if (!is_positive_definite_matrix(some_matrix)) {
    perror("Warning, not a positive-definite matrix.\n");
    return result_matrix;
  }

  result_matrix = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == result_matrix.matrix_ptr) {
    return result_matrix;
  }

  for (row = 0; row != some_matrix.rows; ++row) {

    for (col = 0; col < (row + 1); ++col) {

      sum = 0.0L;
      for (i = 0; i < col; ++i) {

	sum += result_matrix.matrix_ptr[row][i] * 
	  result_matrix.matrix_ptr[col][i];
      }

      if ((row != col) && (fabsl(result_matrix.matrix_ptr[col][col]) - 0.00000000L < 0.000000000L)) {
	matrix_free_memory(&result_matrix);
	return result_matrix;
      }

      result_matrix.matrix_ptr[row][col] = (row == col) ? 
	sqrtl(some_matrix.matrix_ptr[row][col] - sum)    :
	(1 / result_matrix.matrix_ptr[col][col]) * (some_matrix.matrix_ptr[row][col] - sum);
    }
  }

  return result_matrix;
}

long long int matrix_classify(matrix some_matrix)
{
  long long int matrix_type = 0LL, iterator;
  bool (* const func_ptr_array[])(matrix some_matrix) = {

    is_identity_matrix, is_zero_matrix, is_binary_matrix, 
    is_anti_diagonal_matrix, is_arrowhead_matrix, is_markov_matrix,
    is_left_stochastic_matrix, is_doubly_stochastic_matrix, 
    is_tridiagonal_matrix, is_pentadiagonal_matrix, is_positive_matrix,
    is_signature_matrix, is_toeplitz_matrix, is_walsh_matrix,
    is_vandermonde_matrix, is_involutory_matrix, is_alternating_sign_matrix,
    is_hankel_matrix, is_circulant_matrix, is_frobenius_matrix,
    is_metzler_matrix, is_monomial_matrix, is_skew_symmetric_matrix,
    is_symplectic_matrix, is_idempotent_matrix, is_orthogonal_matrix,
    is_permutation_matrix, is_symmetric_matrix, is_positive_definite_matrix,
    is_invertible_matrix, is_sparse_matrix, is_upper_shift_matrix, is_lower_shift_matrix,
    is_shift_matrix, is_lehmer_matrix,
    NULL
  };

  for (iterator = 0; func_ptr_array[iterator] != NULL; ++iterator) {

    if (func_ptr_array[iterator](some_matrix)) {

      matrix_type |= (1LL << iterator);
    }
  }

  return matrix_type;
}

void matrix_print_type(long long int matrix_type)
{
  long long int iterator;
  const char *const matrix_names[] = {

    "Identity matrix", "Zero matrix", "Binary matrix", "Anti-diagonal matrix",
    "Arrowhead matrix", "Markov matrix / Right stochastic matrix",
    "Left stochastic matrix", "Doubly stochastic matrix",
    "Tridiagonal matrix", "Pentadiagonal matrix", "Positive matrix",
    "Signature matrix", "Toeplitz matrix", "Walsh matrix",
    "Vandermonde matrix", "Involutory Matrix", "Alternating sign matrix",
    "Hankel matrix", "Circulant matrix", "Frobenius matrix", "Metzler matrix",
    "Monomial matrix", "Skew-symmetric matrix", "Symplectic matrix",
    "Idempotent matrix", "Orthogonal matrix / Unitary matrix", "Permutation matrix", 
    "Symmetric matrix / Hermitian matrix", "Positive-definite matrix", 
    "Invertible matrix / Non-singular", "Sparse matrix", "Upper shift matrix",
    "Lower shift matrix", "Shift matrix", "Lehmer matrix",
    ""
  };

  for (iterator = 0; *matrix_names[iterator]; ++iterator) {

    if (matrix_type & (1LL << iterator)) {

      printf("%s\n", matrix_names[iterator]);
    }
  }
}

bool is_identity_matrix(matrix some_matrix)
{
  int row, column;
  bool verdict = true;

  if (NULL == some_matrix.matrix_ptr) {
    return false;
  }
  else if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  for (row = 0; (row < some_matrix.rows) && verdict; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      if (row == column) {

	if (some_matrix.matrix_ptr[row][column] != 1.0L) {
	  verdict = false;
	  break;
	}
      }
      else {

	if (some_matrix.matrix_ptr[row][column] != 0.0L) {
	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}

bool is_zero_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, column;

  if (NULL == some_matrix.matrix_ptr) {
    return false;
  }
  else if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      if (some_matrix.matrix_ptr[row][column] != 0.0L) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_binary_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, column;

  if (NULL == some_matrix.matrix_ptr) {

    return false;
  }
  else if (some_matrix.rows != some_matrix.columns) {

    return false;
  }

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      if ((some_matrix.matrix_ptr[row][column] == 0.0L) || 
	  (some_matrix.matrix_ptr[row][column] == 1.0L)) {
	
	continue;
      }
      else {

	verdict = false;
	break;
      }
    }
  }

  return verdict;  
}

bool is_anti_diagonal_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, column;

  for (row = 0; (row < some_matrix.rows) && verdict; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      if ((row + column) == (some_matrix.rows - 1)) {

	if (some_matrix.matrix_ptr[row][column] == 0.0L) {

	  verdict = false;
	  break;
	}
      }
      else {

	if (some_matrix.matrix_ptr[row][column] != 0.0L) {

	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}

bool is_arrowhead_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, column;

  if (some_matrix.rows != some_matrix.columns)
    return false;

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (column = 0; column < some_matrix.columns; ++column) {

      if (!row || !column || row == column) {

	if (some_matrix.matrix_ptr[row][column] == 0.0L) {

	  verdict = false;
	  break;
	}
      }
      else {

	if (some_matrix.matrix_ptr[row][column] != 0.0L) {

	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}

bool is_markov_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, column;
  long double sum;

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    sum = 0.0L;

    for (column = 0; column < some_matrix.columns; ++column) {

      if (some_matrix.matrix_ptr[row][column] < 0) {

	verdict = false;
	break;
      }
      else {

	sum += some_matrix.matrix_ptr[row][column];
      }
    }

    if (sum != 1.0L) {

      verdict = false;
      break;
    }
  }

  return verdict;
}

bool is_left_stochastic_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;
  long double sum;

  for (col = 0; col < some_matrix.columns && verdict; ++col) {

    sum = 0.0L;

    for (row = 0; row < some_matrix.rows; ++row) {

      if (some_matrix.matrix_ptr[row][col] < 0) {

	verdict = false;
	break;
      }
      else {

	sum += some_matrix.matrix_ptr[row][col];
      }
    }

    if (sum != 1.0L) {

      verdict = false;
      break;
    }
  }

  return verdict;
}

bool is_doubly_stochastic_matrix(matrix some_matrix)
{
  return is_markov_matrix(some_matrix) && 
    is_left_stochastic_matrix(some_matrix);
}

bool is_tridiagonal_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row != some_matrix.rows) && verdict; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if (row == col) {

	if (0.0 == some_matrix.matrix_ptr[row][col]) {

	  verdict = false;
	  break;
	}
      }
      else if ((row == col + 1) || (col == row + 1)) {

	if (0.0L == some_matrix.matrix_ptr[row][col]) {

	  verdict = false;
	  break;
	}
      }
      else {

	if (some_matrix.matrix_ptr[row][col] != 0.0L) {

	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}

bool is_pentadiagonal_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col, i;

  for (row = 0; (row != some_matrix.rows) && verdict; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      for (i = 0; i != 5; ++i) {

	if ((row == col + i) || (col == row + i)) {

	  if (0.0L == some_matrix.matrix_ptr[row][col]) {

	    verdict = false;
	    break;
	  }
	}
	else {

	  if (some_matrix.matrix_ptr[row][col] != 0.0L) {

	    verdict = false;
	    break;
	  }
	}
      }
    }
  }

  return verdict;
}

bool is_positive_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] < 0) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_signature_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  if (some_matrix.rows != some_matrix.columns) {

    return false;
  }

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (row == col) {

	if (some_matrix.matrix_ptr[row][col] == 1.0L ||
	    some_matrix.matrix_ptr[row][col] == -1.0L) {

	  continue;
	}
	else {

	  verdict = false;
	  break;
	}
      }
      else {

	if (some_matrix.matrix_ptr[row][col] != 0.0L) {

	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}

bool is_toeplitz_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row < some_matrix.rows - 1) && verdict; ++row) {

    for (col = 0; col < some_matrix.columns - 1; ++col) {

      if (some_matrix.matrix_ptr[row][col] !=
	  some_matrix.matrix_ptr[row + 1][col + 1]) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_walsh_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  if (some_matrix.rows != some_matrix.columns) {

    return false;
  }
  else if (powl(some_matrix.rows, 1.0L / some_matrix.rows) != 2.0L) {

    return false;
  }

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] == 1.0L ||
	  some_matrix.matrix_ptr[row][col] == -1.0L) {

	continue;
      }
      else {

	verdict = false;
	break;
      }
    }
  }

  //dot product

  return verdict;
}

bool is_vandermonde_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (powl(some_matrix.matrix_ptr[row][col], col) != 
	  some_matrix.matrix_ptr[row][col]) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_involutory_matrix(matrix some_matrix)
{
  bool verdict = false;
  matrix square, identity;

  if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  square = matrix_power(some_matrix, 2);
  identity = identity_matrix(some_matrix.rows);

  if (NULL == square.matrix_ptr) {

    matrix_free_memory(&identity);
    return false;
  }
  if (NULL == identity.matrix_ptr) {

    matrix_free_memory(&square);
    return false;
  }

  if (matrix_equality(identity, square)) {

    verdict = true;
  }

  matrix_free_memory(&identity);
  matrix_free_memory(&square);

  return verdict;
}

bool is_alternating_sign_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;
  long double sum;
  enum item_history { not_found, positive, negative} item, prev_state;

  if (some_matrix.rows != some_matrix.columns) {

    return false;
  }

  for (row = 0; row < some_matrix.rows; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] == 0.0 ||
	  some_matrix.matrix_ptr[row][col] == 1.0 ||
	  some_matrix.matrix_ptr[row][col] == -1.0) {

	continue;
      }
      else {

	return false;
      }
    }
  }

  for (row = 0; row < some_matrix.rows; ++row) {

    sum = 0.0;
    item = not_found;

    for (col = 0; col < some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] != 0.0) {

	prev_state = item;
	item = (some_matrix.matrix_ptr[row][col] > 0) ? positive : negative;

	if (item == prev_state) {

	  return false;
	}
      }

      sum += some_matrix.matrix_ptr[row][col];
    }

    if (sum != 1.0) {

      return false;
    }
  }

  for (col = 0; row < some_matrix.columns; ++col) {

    sum = 0.0;
    item = not_found;

    for (row = 0; row < some_matrix.rows; ++row) {

      if (some_matrix.matrix_ptr[row][col] != 0.0) {

	prev_state = item;
	item = (some_matrix.matrix_ptr[row][col] > 0) ? positive : negative;

	if (item == prev_state) {

	  return false;
	}
      }

      sum += some_matrix.matrix_ptr[row][col];
    }

    if (sum != 1.0) {

      verdict = false;
      break;
    }
  }

  return verdict;
}

bool is_hankel_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  if (some_matrix.rows != some_matrix.columns) {

    return false;
  }

  for (row = 0; row < some_matrix.rows; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (!row) {

	continue;
      }
      else {
	
	if (some_matrix.matrix_ptr[row][col] != 
	    some_matrix.matrix_ptr[row - 1][col + 1]) {

	  return false;
	}
      }
    }
  }

  return verdict;
}

bool is_circulant_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row < some_matrix.rows - 1) && verdict; ++row) {

    for (col = 0; col < some_matrix.columns - 1; ++col) {

      if ((row != 0) && !col) {

	if (some_matrix.matrix_ptr[row][col] != 
	    some_matrix.matrix_ptr[row - 1][some_matrix.columns - 1]) {
	  verdict = false;
	  break;
	}
      }
      else if (some_matrix.matrix_ptr[row][col] !=
	   some_matrix.matrix_ptr[row + 1][col + 1]) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_frobenius_matrix(matrix some_matrix)
{
  bool verdict = true, col_traverse = false;
  int row, col;

  if (some_matrix.rows != some_matrix.columns) {

    return false;
  }

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (row == col) {

	if (some_matrix.matrix_ptr[row][col] != 1.0) {

	  verdict = false;
	  break;
	}
	else if (row > col) {

	  if (col_traverse && some_matrix.matrix_ptr[row][col] != 0.0) {

	    return false;
	  }
	  else {

	    if (some_matrix.matrix_ptr[row][col] != 0.0) {

	      col_traverse = true;
	    }
	  }
	}
      }
    }
  }

  return verdict;
}

bool is_metzler_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; row < some_matrix.rows && verdict; ++row) {

    for (col = 0; col < some_matrix.columns; ++col) {

      if (row == col) {

	continue;
      }
      else {

	if (some_matrix.matrix_ptr[row][col] < 0) {

	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}

bool is_monomial_matrix(matrix some_matrix)
{
  bool verdict = true, visited;
  int row, col;

  for (row = 0; row < some_matrix.rows; ++row) {

    visited = false;

    for (col = 0; col < some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] != 0.0) {

	if (visited) {

	  return false;
	}
	else {

	  visited = true;
	}
      }
    }
  }

  for (col = 0; col < some_matrix.columns; ++col) {

    visited = false;

    for (row = 0; row < some_matrix.rows; ++row) {

      if (some_matrix.matrix_ptr[row][col] != 0.0) {

	if (visited) {

	  return false;
	}
	else {

	  visited = true;
	}
      }
    }
  }

  return verdict;
}

bool is_skew_symmetric_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row != some_matrix.rows) && verdict; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] != -some_matrix.matrix_ptr[col][row]) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_symplectic_matrix(matrix some_matrix)
{
  bool verdict = true;
  matrix omega, identity, product, transpose;
  int row, col;

  if ((some_matrix.rows % 2) || (some_matrix.columns % 2)) {
    return false;
  }

  omega = matrix_allocate_memory(some_matrix.rows, some_matrix.columns);

  if (NULL == omega.matrix_ptr) {
    return false;
  }

  identity = identity_matrix(some_matrix.columns / 2);

  if (NULL == identity.matrix_ptr) {
    matrix_free_memory(&omega);
    return false;
  }

  for (row = 0; row != identity.rows; ++row) {

    for (col = 0; col != identity.columns; ++col) {

      omega.matrix_ptr[row][col + omega.columns / 2] = identity.matrix_ptr[row][col];
      omega.matrix_ptr[row + identity.rows][col] = -identity.matrix_ptr[row][col];
    }
  }

  matrix_free_memory(&identity);
  transpose = matrix_transposition(some_matrix);

  if (NULL == transpose.matrix_ptr) {
    matrix_free_memory(&omega);
    return false;
  }

  product = matrix_multiplication(transpose, omega);
  matrix_free_memory(&transpose);

  if (NULL == product.matrix_ptr) {
    matrix_free_memory(&omega);
    return false;
  }

  transpose = product;
  product = matrix_multiplication(product, some_matrix);
  matrix_free_memory(&transpose);

  if (NULL == product.matrix_ptr) {
    matrix_free_memory(&omega);
    return false;
  }

  if (!matrix_equality(omega, product)) {
    verdict = false;
  }

  matrix_free_memory(&product);
  matrix_free_memory(&omega);

  return verdict;
}

bool is_idempotent_matrix(matrix some_matrix)
{
  bool verdict = true;
  matrix test_matrix;

  if (some_matrix.columns != some_matrix.rows) {
    return false;
  }

  test_matrix = matrix_multiplication(some_matrix, some_matrix);

  if (NULL == test_matrix.matrix_ptr) {
    return false;
  }

  if (!matrix_equality(some_matrix, test_matrix)) {

    verdict = false;
  }

  matrix_free_memory(&test_matrix);

  return verdict;
}

bool is_orthogonal_matrix(matrix some_matrix)
{
  bool verdict = true;
  matrix test_matrix, identity, multiplication_result;

  if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  test_matrix = matrix_transposition(some_matrix);

  if (NULL == test_matrix.matrix_ptr) {
    return false;
  }

  multiplication_result = matrix_multiplication(some_matrix, test_matrix);

  if (NULL == multiplication_result.matrix_ptr) {
    matrix_free_memory(&test_matrix);
    return false;
  }

  identity = identity_matrix(some_matrix.rows);

  if (NULL == identity.matrix_ptr) {
    matrix_free_memory(&test_matrix);
    matrix_free_memory(&multiplication_result);
    return false;
  }

  if (!matrix_equality(identity, multiplication_result)) {

    verdict = false;
  }

  matrix_free_memory(&identity);
  matrix_free_memory(&multiplication_result);
  matrix_free_memory(&test_matrix);

  return verdict;
}

bool is_permutation_matrix(matrix some_matrix)
{
  bool verdict = true, visited;
  int row, col;

  for (row = 0; row < some_matrix.rows; ++row) {

    visited = false;

    for (col = 0; col < some_matrix.columns; ++col) {

      if (some_matrix.matrix_ptr[row][col] != 0.0) {

	if (some_matrix.matrix_ptr[row][col] != 1.0) {

	  return false;
	}

	if (visited) {

	  return false;
	}
	else {

	  visited = true;
	}
      }
    }
  }

  for (col = 0; col < some_matrix.columns; ++col) {

    visited = false;

    for (row = 0; row < some_matrix.rows; ++row) {

      if (some_matrix.matrix_ptr[row][col] != 0.0) {

	if (some_matrix.matrix_ptr[row][col] != 1.0) {

	  return false;
	}

	if (visited) {

	  return false;
	}
	else {

	  visited = true;
	}
      }
    }
  }

  return verdict;
}

bool is_symmetric_matrix(matrix some_matrix)
{
  matrix transpose;
  bool verdict = true;

  if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  transpose = matrix_transposition(some_matrix);

  if (NULL == transpose.matrix_ptr) {
    return false;
  }

  verdict = matrix_equality(some_matrix, transpose);
  matrix_free_memory(&transpose);

  return verdict;
}

bool is_positive_definite_matrix(matrix some_matrix)
{
  matrix eigenvalues;
  bool verdict = true;
  int col;

  if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  eigenvalues = qr_iteration_for_eigenvalues(some_matrix);

  if (NULL == eigenvalues.matrix_ptr) {
    return false;
  }

  for (col = 0; col != eigenvalues.columns; ++col) {

    if (eigenvalues.matrix_ptr[0][col] < 0.0L) {
      verdict = false;
      break;
    }
  }

  matrix_free_memory(&eigenvalues);

  return verdict;
}

bool is_invertible_matrix(matrix some_matrix)
{
  bool verdict = true;
  long double determinant;

  if (some_matrix.rows != some_matrix.columns) {
    return false;
  }

  determinant = matrix_determinant_using_qr(some_matrix);
  
  if ((0.0L == determinant) || isnan(determinant)) {

    verdict = false;
  }

  return verdict;
}

bool is_sparse_matrix(matrix some_matrix)
{
  bool verdict = false;
  int row, col, zero_count = 0;

  if (NULL == some_matrix.matrix_ptr) {
    return false;
  }
  
  for (row = 0; row != some_matrix.rows; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if (0.0L == some_matrix.matrix_ptr[row][col]) {
	++zero_count;
      }
    }
  }

  if (((long double) zero_count / (some_matrix.rows * some_matrix.columns)) > 0.5L) {
    verdict = true;
  }
  
  return verdict;
}

bool is_upper_shift_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row != some_matrix.rows) && verdict; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if ((row + 1) == col) {
	
	if (some_matrix.matrix_ptr[row][col] != 1.0L) {

	  verdict = false;
	  break;
	}
      }
      else if (some_matrix.matrix_ptr[row][col] != 0.0L) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_lower_shift_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row != some_matrix.rows) && verdict; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if ((col + 1) == row) {
	
	if (some_matrix.matrix_ptr[row][col] != 1.0L) {

	  verdict = false;
	  break;
	}
      }
      else if (some_matrix.matrix_ptr[row][col] != 0.0L) {

	verdict = false;
	break;
      }
    }
  }

  return verdict;
}

bool is_shift_matrix(matrix some_matrix)
{
  return is_upper_shift_matrix(some_matrix) || is_lower_shift_matrix(some_matrix);
}

bool is_lehmer_matrix(matrix some_matrix)
{
  bool verdict = true;
  int row, col;

  for (row = 0; (row != some_matrix.rows) && verdict; ++row) {

    for (col = 0; col != some_matrix.columns; ++col) {

      if (col >= row) {

	if (some_matrix.matrix_ptr[row][col] != ((long double) (row + 1)) / (col + 1)) {
	  verdict = false;
	  break;
	}
      }
      else {

	if (some_matrix.matrix_ptr[row][col] != ((long double) (col + 1)) / (row + 1)) {
	  verdict = false;
	  break;
	}
      }
    }
  }

  return verdict;
}
