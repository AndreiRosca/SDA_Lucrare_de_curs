#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <windows.h>
#include "matrix_interface.h"

void print_menu(const char *menu[]);
matrix matrix_read(void);
matrix matrix_buffer_read(char *memory_buffer);
void matrix_buffer_print(matrix some_matrix, HWND hwndDlgItem);
void matrix_print(matrix some_matrix);
void print_matrix_type(long long int matrix_type, HWND hwndWnd);


#define MAX_MATRIX_BUFFER_SIZE 1024
#define USER_INPUT_SIZE 4

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define flush() \
  while (getchar() != '\n') \
    continue;

#endif
