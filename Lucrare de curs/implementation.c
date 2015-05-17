#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "interface.h"
#include <windows.h>
#include "matrix_interface.h"

bool parenthesis_check_matching(char *some_buffer);

void print_menu(const char *menu[])
{
  int iterator;

  for (iterator = 0; *menu[iterator]; ++iterator) {

    printf("%2i. %s\n", iterator + 1, menu[iterator]);
  }

  printf("\n");
}

matrix matrix_read(void)
{
  char matrix_buffer[MAX_MATRIX_BUFFER_SIZE];
  char *buffer_ptr = matrix_buffer, *end = NULL;
  matrix some_matrix = { .matrix_ptr = NULL };
  int total_rows = 1, total_cols = 1, row, col;
  int prev_num_cols = -1, col_count = 1;
  bool inside_row = false, invalid_format = false, met_digit;

  fgets(matrix_buffer, MAX_MATRIX_BUFFER_SIZE, stdin);

  if (!parenthesis_check_matching(matrix_buffer)) {
    return some_matrix;
  }

  for ( ; *buffer_ptr; ++buffer_ptr) {

    if (isspace(*buffer_ptr)) {
      continue;
    }

    if (*buffer_ptr == '[') {

      *buffer_ptr = ' ';
      col_count = 1;
      inside_row = true;
      met_digit = false;
    }
    else if (*buffer_ptr == ']') {

      *buffer_ptr = ' ';
      inside_row = false;

      if (-1 == prev_num_cols) {

	prev_num_cols = col_count;
      }
      else if(prev_num_cols != col_count) {

	invalid_format = true;	
	printf("Invalid matrix format -- try again.\n");
	break;
      }
      else {

	prev_num_cols = col_count;
      }
    }
    else if (*buffer_ptr == ',') {

      if (inside_row) {

	if (!met_digit) {

	  invalid_format = true;
	  printf("Invalid matrix format -- try again.\n");
	  break;
	}

	++col_count;
	met_digit = false;
      }
      else {

	++total_rows;
      }

      *buffer_ptr = ' ';
    }
    else if (!met_digit && isdigit(*buffer_ptr)) {

      met_digit = true;
    }
  }

  total_cols = prev_num_cols;

  if (invalid_format) {
    return some_matrix;
  }

  for (buffer_ptr = matrix_buffer; *buffer_ptr; ++buffer_ptr) {

    if (isspace(*buffer_ptr)) {
      continue;
    }
    else if (!isdigit(*buffer_ptr) && *buffer_ptr != '-' && *buffer_ptr != '.' &&
	     *buffer_ptr != 'e' && *buffer_ptr != 'E') {
      printf("Warning, unexpected characters in matrix -- try again.\n");
      return some_matrix;
    }
  }

  some_matrix = matrix_allocate_memory(total_rows, total_cols);

  if (NULL == some_matrix.matrix_ptr) {
    printf("Sorry, memory allocation failure -- try again later.\n");
    return some_matrix;
  }

  buffer_ptr = matrix_buffer, row = 0, col = 0;
  for (some_matrix.matrix_ptr[row][col] = strtold(buffer_ptr, &end);
       buffer_ptr != end; some_matrix.matrix_ptr[row][col] = strtold(buffer_ptr, &end)) {

    buffer_ptr = end;

    if (errno == ERANGE) {

      printf("Warning, out of range value found -- try again.\n");
      matrix_free_memory(&some_matrix);
      errno = 0;
      return some_matrix;
    }

    ++col;
    if (col == total_cols) {

      col = 0;
      ++row;

      if (row == total_rows) {

	break;
      }
    }
  }

  return some_matrix;
}

matrix matrix_buffer_read(char *memory_buffer)
{
  char *buffer_ptr = memory_buffer, *end = NULL;
  matrix some_matrix = { .matrix_ptr = NULL };
  int total_rows = 1, total_cols = 1, row, col;
  int prev_num_cols = -1, col_count = 1;
  bool inside_row = false, invalid_format = false, met_digit;

   if (!parenthesis_check_matching(memory_buffer)) {
    return some_matrix;
  }

  for ( ; *buffer_ptr; ++buffer_ptr) {

    if (isspace(*buffer_ptr)) {
      continue;
    }

    if (*buffer_ptr == '[') {

      *buffer_ptr = ' ';
      col_count = 1;
      inside_row = true;
      met_digit = false;
    }
    else if (*buffer_ptr == ']') {

      *buffer_ptr = ' ';
      inside_row = false;

      if (-1 == prev_num_cols) {

	prev_num_cols = col_count;
      }
      else if(prev_num_cols != col_count) {

	invalid_format = true;	
	MessageBox(NULL, "Invalid matrix format -- try again.\n", 0, MB_OK);
	break;
      }
      else {

	prev_num_cols = col_count;
      }
    }
    else if (*buffer_ptr == ',') {

      if (inside_row) {

	if (!met_digit) {

	  invalid_format = true;
	  MessageBox(NULL, "Invalid matrix format -- try again.\n", 0, MB_OK);
	  break;
	}

	++col_count;
	met_digit = false;
      }
      else {

	++total_rows;
      }

      *buffer_ptr = ' ';
    }
    else if (!met_digit && isdigit(*buffer_ptr)) {

      met_digit = true;
    }
  }

  total_cols = prev_num_cols;

  if (invalid_format) {
    return some_matrix;
  }

  for (buffer_ptr = memory_buffer; *buffer_ptr; ++buffer_ptr) {

    if (isspace(*buffer_ptr)) {
      continue;
    }
    else if (!isdigit(*buffer_ptr) && *buffer_ptr != '-' && *buffer_ptr != '.' &&
	     *buffer_ptr != 'e' && *buffer_ptr != 'E') {
      MessageBox(NULL, "Warning, unexpected characters in matrix -- try again.\n", 0, MB_OK);
      return some_matrix;
    }
  }
  /*
  if (!(total_rows - 1) || !(total_cols - 1)) {
    MessageBox(NULL, "Warning, wrong matrix dimensions.\n", NULL, MB_OK);
    return some_matrix;
  }
  */
  some_matrix = matrix_allocate_memory(total_rows, total_cols);

  if (NULL == some_matrix.matrix_ptr) {
    MessageBox(NULL, "Sorry, memory allocation failure -- try again later.\n", 0, MB_OK);
    return some_matrix;
  }

  buffer_ptr = memory_buffer, row = 0, col = 0;
  for (some_matrix.matrix_ptr[row][col] = strtold(buffer_ptr, &end);
       buffer_ptr != end; some_matrix.matrix_ptr[row][col] = strtold(buffer_ptr, &end)) {

    buffer_ptr = end;

    ++col;
    if (col == total_cols) {

      col = 0;
      ++row;

      if (row == total_rows) {

	break;
      }
    }
  }

  return some_matrix;
}

void matrix_print(matrix some_matrix)
{
  int row, col;

  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  printf("[");

  for (row = 0; row < some_matrix.rows; ++row) {

    printf("[");
    for (col = 0; col < some_matrix.columns; ++col) {

      printf("%.3Lf%s", some_matrix.matrix_ptr[row][col], 
	     ((col + 1) == some_matrix.columns) ? "" : ", ");
    }
    printf("]%s", ((row + 1) == some_matrix.rows) ? "" : ", ");
  }

  printf("]\n");
}

void matrix_buffer_print(matrix some_matrix, HWND hwndDlgItem)
{
  char number_buffer[MAX_MATRIX_BUFFER_SIZE];
  int row, col;

  if (NULL == some_matrix.matrix_ptr) {
    return;
  }

  SendMessage(hwndDlgItem, EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
  SendMessage(hwndDlgItem, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "[");

  for (row = 0; row < some_matrix.rows; ++row) {

    SendMessage(hwndDlgItem, EM_SETSEL, (WPARAM) -1, 0);
    SendMessage(hwndDlgItem, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "[");

    for (col = 0; col < some_matrix.columns; ++col) {

      snprintf(number_buffer, MAX_MATRIX_BUFFER_SIZE - 1, "%.3Lf",  some_matrix.matrix_ptr[row][col]);
      lstrcat(number_buffer, ((col + 1) == some_matrix.columns) ? "" : ", ");
      SendMessage(hwndDlgItem, EM_SETSEL, (WPARAM) -1, 0);
      SendMessage(hwndDlgItem, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) number_buffer);
    }

    SendMessage(hwndDlgItem, EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
    SendMessage(hwndDlgItem, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "]");
    SendMessage(hwndDlgItem, EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
    SendMessage(hwndDlgItem, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) (((row + 1) == some_matrix.rows) ? "" : ", "));
  }

  SendMessage(hwndDlgItem, EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
  SendMessage(hwndDlgItem, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "]\r\n");
}

bool parenthesis_check_matching(char *some_buffer)
{
  char *parenthesis = NULL;
  int index = -1;
  bool result = true;

  parenthesis = (char *) malloc(sizeof(char) * strlen(some_buffer));

  if (NULL == parenthesis) {
    return false;
  }

  for( ; *some_buffer; ++some_buffer) {

    if(*some_buffer == '(' || *some_buffer == '[' || *some_buffer == '{') {
      
      switch(*some_buffer) {

      case '(':
	parenthesis[++index] = ')';
	break;
	
      case '[':
	parenthesis[++index] = ']';
	break;
	
      case '{':
	parenthesis[++index] = '}';
	break;
      }

      continue;
    }
    else if(*some_buffer == ')' || *some_buffer == ']' || *some_buffer == '}') {

      if(!(index + 1)) {

	result = false;
	break;
      }
      else if(*some_buffer != parenthesis[index]) {

	result = false;
	break;
      }
      else {

	--index;
      }
    }
  }

  free(parenthesis);

  return result;
}

void print_matrix_type(long long int matrix_type, HWND hwndWnd)
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

      SendMessage(hwndWnd, EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
      SendMessage(hwndWnd, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) matrix_names[iterator]);
      SendMessage(hwndWnd, EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
      SendMessage(hwndWnd, EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\n");
    }
  }
}
