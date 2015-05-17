#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "matrix_interface.h"
#include "interface.h"

BOOL WINAPI DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI InputDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WindowProc(HWND hwndWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI binaryEditWindowProc(HWND hwndWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI IconControlWndProc(HWND hwndWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadFunc(LPVOID threadParam);
DWORD WINAPI AboutThreadFunc(LPVOID threadParam);

typedef struct subclass_data {

  int itemId;
  HWND itemHwnd;
  LONG oldWindowProc;
  LONG current_color;
  bool hasFocus;

} subclass_data;

enum controls_id { FIRST_EDIT_ID = 0x220, SECOND_EDIT_ID = 0x221, MATRIX_INPUT_ID = 0x222,
		   MATRIX_CANCEL_ID = 0x223, SECOND_CLEAR_ID = 0x224 };

#define TOTAL_ITEMS 8
#define ADD_ID 0x217
#define MUL_ID 0x216
#define SYSTEM_ID 0x218
#define DIRECT_SUM_ID 0x227
#define KRONECKER_PRODUCT_ID 0x228
#define HADAMARD_PRODUCT_ID 0x229
#define KRONECKER_SUM_ID 0x230
#define FROBENIUS_PRODUCT_ID 0x231

#define BINARY_DLG_TOTAL_ITEMS 2
#define MAX_BUFFER_SIZE 64
#define MAX_FILE_NAME 256
#define MENU_OPEN_ID 0x240

const int MATRIX_ICON_ID = 0x212;
const int MATRIX_CLEAR_ID = 0x215;
const int MATRIX_EDIT_ID  = 0x314;
const int active_color = 0x00000FF;
subclass_data subclassed_items[TOTAL_ITEMS] = { { .itemId =  ADD_ID, .current_color = 0x0FF0000 }, 
                                              { .itemId = SYSTEM_ID, .current_color = 0x0FF0000 }, { .itemId = MUL_ID, .current_color = 0x0FF0000 },
                                              { .itemId = DIRECT_SUM_ID, .current_color = 0x0FF0000 }, 
                                              { .itemId = KRONECKER_PRODUCT_ID, .current_color = 0x0FF0000 }, 
                                              { .itemId = HADAMARD_PRODUCT_ID, .current_color = 0x0FF0000 },
                                              { .itemId = KRONECKER_SUM_ID, .current_color = 0x0FF0000 },
                                              { .itemId = FROBENIUS_PRODUCT_ID, .current_color = 0x0FF0000 }};
subclass_data binary_subclassed_items[BINARY_DLG_TOTAL_ITEMS] = { { .itemId = FIRST_EDIT_ID, .hasFocus = false }, 
								  { .itemId = SECOND_EDIT_ID, .hasFocus = false } };
const char *inputDlgName = "BinaryDialog", *aboutDlgName = "AboutDialog";
int controlId = 0;
HWND mainHwnd;
LONG oldIconWndProc = NULL;
HINSTANCE libraryHinst = 0;
FARPROC playSoundAddr = NULL;

int main()
{
  const char *dialogName = "MatrixDialog";
  HANDLE hModule;

  libraryHinst = LoadLibrary("winmm.dll");
  playSoundAddr = GetProcAddress(libraryHinst, "PlaySoundA");
  hModule = GetModuleHandle(0);
  DialogBoxParam(hModule, dialogName, NULL, DialogProc, (LPARAM) NULL);
  FreeLibrary(libraryHinst);

  return 0;
}

BOOL WINAPI DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  const int MATRIX_EXIT_ID = 0x210, QR_DECOMPOSITION_ID = 0x200, EIGENVALUES_ID = 0x209,
    CHOLESKY_DECOMPOSITION_ID = 0x201, MATRIX_CALCULATE_ID = 0x2718, TRACE_ID  = 0x205, TRANSPOSE_ID = 0x207,
    LU_DECOMPOSITION_ID = 0x202, LUP_DECOMPOSITION_ID = 0x203, MATRIX_RANK_ID = 0x213, DETERMINANT_ID = 0x206, EXP_ID = 0x214,
    MATRIX_CLASSIFY_ID = 0x204, INVERSE_ID = 0x208, MENU_FONT_ID = 0x243, MENU_SAVE_ID = 0x244, MENU_ABOUT_ID = 0x242;
  char convert_buffer[MAX_BUFFER_SIZE];
  int buffer_size, i, rank;
  static char file_name_buffer[MAX_FILE_NAME];
  static char *FileFilterStr[] = { "All files", "*.*", "Text files", "*.txt\0\0"};
  long double determinant, trace;
  const int main_color = 0x0FF0000;
  LPVOID memory_buffer;
  HBRUSH background = 0;
  LOGFONT edit_font = { .lfHeight = 0, .lfWidth = 0, .lfEscapement = 0, .lfOrientation = 0, .lfWeight = FW_DONTCARE,
			.lfItalic = FALSE, .lfUnderline = FALSE, .lfStrikeOut = FALSE, .lfCharSet = DEFAULT_CHARSET,
			.lfOutPrecision = OUT_DEFAULT_PRECIS, .lfClipPrecision = CLIP_DEFAULT_PRECIS, .lfQuality = DEFAULT_QUALITY,
			.lfPitchAndFamily = DEFAULT_PITCH, .lfFaceName = "Courier New" };
  HFONT Hfont = (HFONT) NULL;
  HMENU hMenu = (HMENU) 0;
  HANDLE hFile;
  DWORD file_size = 0, bytes_read = 0, threadId;
  LPVOID file_buffer = NULL;
  OPENFILENAME ofn = { .lStructSize = sizeof(OPENFILENAME), .hwndOwner = hwndDlg, .lpstrFilter = FileFilterStr[0], .lpstrCustomFilter = NULL,
		       .nMaxCustFilter = 0, .nFilterIndex = 0, .lpstrFile = file_name_buffer, .nMaxFile =  MAX_FILE_NAME, 
		       .lpstrFileTitle = NULL, .nMaxFileTitle = 0, .lpstrInitialDir = NULL, .lpstrTitle = NULL, 
		       .Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, .nFileOffset = 0, .nFileExtension = 0,
		       .lpstrDefExt = NULL, .lCustData = 0, .lpfnHook = 0, .lpTemplateName = NULL };

  CHOOSEFONT choose_font = { .lStructSize = sizeof(CHOOSEFONT), .hDC = 0, .lpLogFont = &edit_font, .iPointSize = 0, 
			     .Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS, .rgbColors = 0, .nFontType = REGULAR_FONTTYPE };
  matrix_pair decomposition;
  matrix_tuple lup;
  matrix first_matrix, second_matrix;

  switch (uMsg) {

  case WM_INITDIALOG:

    ofn.hInstance = GetModuleHandle(NULL);
    choose_font.hwndOwner = hwndDlg;
    Hfont = CreateFontIndirect(&edit_font);
    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), WM_SETFONT, (WPARAM) Hfont, (LPARAM) TRUE);
    SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(MATRIX_ICON_ID)));
    CheckRadioButton(hwndDlg, QR_DECOMPOSITION_ID, EIGENVALUES_ID, TRACE_ID);
    hMenu = LoadMenu(GetModuleHandle(NULL), "MatrixMenu");
    SetMenu(hwndDlg, hMenu);

    for (i = 0; i != TOTAL_ITEMS; ++i) {
      subclassed_items[i].itemHwnd = GetDlgItem(hwndDlg, subclassed_items[i].itemId);
      subclassed_items[i].oldWindowProc = SetWindowLong(GetDlgItem(hwndDlg, subclassed_items[i].itemId), 
							GWL_WNDPROC, (LONG) WindowProc);
    }
    mainHwnd = hwndDlg;
    SetFocus(GetDlgItem(hwndDlg,  MATRIX_EDIT_ID));
    break;

  case WM_LBUTTONDOWN:
    
    ReleaseCapture();
    SendMessage(hwndDlg, WM_SYSCOMMAND, (WPARAM) 61458, (LPARAM) 0);
    break;

  case WM_CTLCOLORSTATIC:

    if (!background) {
      background = CreateSolidBrush((COLORREF) GetSysColor(COLOR_BTNFACE));
    }

    for (i = 0; i != TOTAL_ITEMS; ++i) {

      if ((HWND) lParam == subclassed_items[i].itemHwnd) {

	SetBkMode((HDC) wParam, TRANSPARENT);
	SetTextColor((HDC) wParam, (COLORREF) subclassed_items[i].current_color);
	return (COLORREF) background;
      }
    }
    break;

  case WM_SETCURSOR:

    for (i = 0; i != TOTAL_ITEMS; ++i) {
      
      if (subclassed_items[i].current_color != main_color) {
	subclassed_items[i].current_color = main_color;
	InvalidateRect(subclassed_items[i].itemHwnd, NULL, TRUE);
      }
    }

    return FALSE;
    break;

  case WM_COMMAND:

    if ((wParam & 0xFFFF) == MATRIX_EXIT_ID) {

      SendMessage(hwndDlg, WM_CLOSE, 0, 0);
      return TRUE;
    }
    else if ((wParam & 0xFFFF) == MATRIX_CALCULATE_ID) {

      buffer_size = GetWindowTextLength(GetDlgItem(hwndDlg, MATRIX_EDIT_ID));

      if (!buffer_size) {
	return TRUE;
      }

      memory_buffer = VirtualAlloc(NULL, buffer_size + 2, MEM_COMMIT, PAGE_READWRITE);

      if (NULL == memory_buffer) {

	MessageBox(hwndDlg, "Sorry, memory allocation failed. Try again.\n", NULL, MB_OK);
	return TRUE;
      }

      GetWindowText(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), memory_buffer, buffer_size + 1);
      first_matrix = matrix_buffer_read(memory_buffer);
      VirtualFree(memory_buffer, buffer_size + 2, MEM_DECOMMIT);
      SetDlgItemText(hwndDlg, MATRIX_EDIT_ID, NULL);

      if (NULL == first_matrix.matrix_ptr) {
	return TRUE;
      }

      if (IsDlgButtonChecked(hwndDlg, TRANSPOSE_ID)) {

	second_matrix = matrix_transposition(first_matrix);

	if (NULL != second_matrix.matrix_ptr) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix transpose:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&second_matrix);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, matrix transposition failed.\n Try later.\n", NULL, MB_OK);
	}
    
      }
      else if (IsDlgButtonChecked(hwndDlg, EXP_ID)) {

	second_matrix = matrix_exponential(first_matrix);

	if (NULL != second_matrix.matrix_ptr) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix exponential:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&second_matrix);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, matrix exponentiation failed.\n Try later.\n", NULL, MB_OK);
	}
   
      }
      else if (IsDlgButtonChecked(hwndDlg, MATRIX_RANK_ID)) {

	rank = matrix_get_rank(first_matrix);

	if (-1 != rank) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix rank: ");
	  wsprintf(convert_buffer, "%i\r\n", rank);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) convert_buffer);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, matrix rank calculation failed.\n Try later.\n", NULL, MB_OK);
	}
    
      }
      else if (IsDlgButtonChecked(hwndDlg, DETERMINANT_ID)) {

	determinant = matrix_determinant_using_qr(first_matrix);
	
	if (isnan(determinant)) {
		determinant = matrix_lup_determinant(first_matrix);
	}

	if (!isnan(determinant)) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix determinant: ");
	  snprintf(convert_buffer, sizeof(convert_buffer) - 1, "%Lf\r\n", determinant);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) convert_buffer);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, matrix determinant calculation failed.\n Try later.\n", NULL, MB_OK);
	}
    
      }
      else if (IsDlgButtonChecked(hwndDlg, TRACE_ID)) {

	trace = matrix_trace(first_matrix);

	if (!isnan(determinant)) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix trace: ");
	  snprintf(convert_buffer, sizeof(convert_buffer) - 1, "%Lf\r\n", trace);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) convert_buffer);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, matrix trace calculation failed.\n Try later.\n", NULL, MB_OK);
	}
      }
      else if (IsDlgButtonChecked(hwndDlg, INVERSE_ID)) {

	second_matrix = matrix_inverse(first_matrix);

	if (NULL != second_matrix.matrix_ptr) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix inverse:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&second_matrix);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, matrix inversion failed.\n Try later.\n", NULL, MB_OK);
	}
      }
      else if (IsDlgButtonChecked(hwndDlg, MATRIX_CLASSIFY_ID)) {

	SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMatrix type:\r\n\r\n");
	print_matrix_type(matrix_classify(first_matrix), GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
      }
      else if (IsDlgButtonChecked(hwndDlg, LUP_DECOMPOSITION_ID)) {

	lup = matrix_lup_decomposition(first_matrix);

	if ((NULL != lup.first.matrix_ptr) && (NULL != lup.second.matrix_ptr) && (NULL != lup.third.matrix_ptr)) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nLUP decomposition:\r\n");
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "L:\r\n\r\n");
	  matrix_buffer_print(lup.first, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nU:\r\n\r\n");
	  matrix_buffer_print(lup.second, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nP:\r\n\r\n");
	  matrix_buffer_print(lup.third, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&lup.first);
	  matrix_free_memory(&lup.second);
	  matrix_free_memory(&lup.third);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, LUP decomposition failed.\n", NULL, MB_OK);
	}
      }
      else if (IsDlgButtonChecked(hwndDlg, LU_DECOMPOSITION_ID)) {

	decomposition = matrix_lu_decomposition(first_matrix);

	if ((NULL != decomposition.first.matrix_ptr) && (NULL != decomposition.second.matrix_ptr)) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nLU decomposition:\r\n");
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "L:\r\n\r\n");
	  matrix_buffer_print(decomposition.first, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nU:\r\n\r\n");
	  matrix_buffer_print(decomposition.second, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&decomposition.first);
	  matrix_free_memory(&decomposition.second);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, LU decomposition failed.\n", NULL, MB_OK);
	}
      }
      else if (IsDlgButtonChecked(hwndDlg, CHOLESKY_DECOMPOSITION_ID)) {

	second_matrix = matrix_cholesky_decomposition(first_matrix);

	if (NULL != second_matrix.matrix_ptr) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nCholesky decomposition:\r\n");
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "L:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));

	  decomposition.first = matrix_transposition(second_matrix);

	  if (NULL != decomposition.first.matrix_ptr) {

	    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nL^T:\r\n\r\n");
	    matrix_buffer_print(decomposition.first, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	    matrix_free_memory(&decomposition.first);
	  }

	  matrix_free_memory(&second_matrix);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, Cholesky decomposition failed.\r\nProbably not a symetric, positive-definde matrix.\r\n", NULL, MB_OK);
	}
      }
      else if (IsDlgButtonChecked(hwndDlg, QR_DECOMPOSITION_ID)) {

	decomposition = matrix_qr_decomposition(first_matrix);

	if ((NULL != decomposition.first.matrix_ptr) && (NULL != decomposition.second.matrix_ptr)) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nQR decomposition:\r\n");
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Q:\r\n\r\n");
	  matrix_buffer_print(decomposition.first, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nR:\r\n\r\n");
	  matrix_buffer_print(decomposition.second, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&decomposition.first);
	  matrix_free_memory(&decomposition.second);
	}
	else {
	  MessageBox(hwndDlg, "Sorry, the QR decomposition failed.\n", NULL, MB_OK);
	  SendMessage(hwndDlg, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) GetDlgItem(hwndDlg, MATRIX_CLEAR_ID));
	}
      }
      else if (IsDlgButtonChecked(hwndDlg, EIGENVALUES_ID)) {

	second_matrix = qr_iteration_for_eigenvalues(first_matrix);

	if (NULL != second_matrix.matrix_ptr) {

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nEigenvalue matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&second_matrix);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, the eigenvalues calculation failed.\n", NULL, MB_OK);
	  SendMessage(hwndDlg, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) GetDlgItem(hwndDlg, MATRIX_CLEAR_ID));
	}

	second_matrix = qr_matrix_eigenvectors(first_matrix);

	if (NULL != second_matrix.matrix_ptr) {

	  buffer_size = GetWindowTextLength(GetDlgItem(hwndDlg, MATRIX_EDIT_ID));

	  if (!buffer_size) {

	    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Input matrix:\r\n\r\n");
	    matrix_buffer_print(first_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  }

	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nEigenvectors matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  matrix_free_memory(&second_matrix);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, eigenvectors calculation failed.\n", NULL, MB_OK);
	  SendMessage(hwndDlg, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) GetDlgItem(hwndDlg, MATRIX_CLEAR_ID));
	}
      }

      matrix_free_memory(&first_matrix);
      SetFocus(GetDlgItem(hwndDlg,  MATRIX_EDIT_ID));

    }
    else if ((wParam & 0xFFFF) == MATRIX_CLEAR_ID) {

      SetDlgItemText(hwndDlg, MATRIX_EDIT_ID, NULL);
      SetFocus(GetDlgItem(hwndDlg,  MATRIX_EDIT_ID));
    }
    else if ((wParam & 0xFFFF) == MENU_FONT_ID) {

      if (ChooseFont(&choose_font)) {

	DeleteObject(Hfont);
	Hfont = CreateFontIndirect(&edit_font);
	SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), WM_SETFONT, (WPARAM) Hfont, (LPARAM) TRUE);
      }

      SetFocus(GetDlgItem(hwndDlg,  MATRIX_EDIT_ID));
    }
    else if (((wParam & 0xFFFF) == MENU_OPEN_ID) && !lParam) {
      
      if (GetOpenFileName(&ofn)) {

	SendMessage(hwndDlg, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) GetDlgItem(hwndDlg, MATRIX_CLEAR_ID));
	hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE != hFile) {

	  file_size = GetFileSize(hFile, NULL);
	  file_buffer = VirtualAlloc(NULL, file_size, MEM_COMMIT, PAGE_READWRITE);

	  if (NULL == file_buffer) {
	    
	    CloseHandle(hFile);
	    MessageBox(hwndDlg, "Sorry, memory allocation failed.\r\nTry again!\n", NULL, MB_OK);
	  }
	  else {

	    ReadFile(hFile, file_buffer, file_size, &bytes_read, NULL);
	    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), WM_SETTEXT, (WPARAM) 0, (LPARAM) file_buffer);
	    VirtualFree(file_buffer, file_size, MEM_DECOMMIT);
	    CloseHandle(hFile);
	  }
	}
	else {

	  MessageBox(hwndDlg, "Sorry, can't open your file.\r\nTry again!\n", NULL, MB_OK);
	}

	SetFocus(GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
      }
    }
    else if (((wParam & 0xFFFF) == MENU_SAVE_ID) && !lParam) {
      
      if (GetSaveFileName(&ofn)) {

	hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE != hFile) {

	  file_size = GetWindowTextLength(GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
	  file_buffer = VirtualAlloc(NULL, file_size, MEM_COMMIT, PAGE_READWRITE);

	  if (NULL == file_buffer) {
	    
	    CloseHandle(hFile);
	    MessageBox(hwndDlg, "Sorry, memory allocation failed.\r\nTry again!\n", NULL, MB_OK);
	  }
	  else {

	    SendMessage(GetDlgItem(hwndDlg, MATRIX_EDIT_ID), WM_GETTEXT, (WPARAM) file_size, (LPARAM) file_buffer);
	    WriteFile(hFile, file_buffer, file_size, &bytes_read, NULL);
	    VirtualFree(file_buffer, file_size, MEM_DECOMMIT);
	    CloseHandle(hFile);
	  }
	}
	else {

	  MessageBox(hwndDlg, "Sorry, can't open your file.\r\nTry again!\n", NULL, MB_OK);
	}

	SetFocus(GetDlgItem(hwndDlg, MATRIX_EDIT_ID));
      }
    }
    else if (((wParam & 0xFFFF) == MENU_ABOUT_ID) && !lParam) {
      
      CreateThread(NULL, 0, AboutThreadFunc, hwndDlg, 0, &threadId);
    }

    return TRUE;
    break;

  case WM_INITMENUPOPUP:

    buffer_size = GetWindowTextLength(GetDlgItem(hwndDlg, MATRIX_EDIT_ID));

    if (!buffer_size) {

      EnableMenuItem((HMENU) wParam, MENU_SAVE_ID, MF_GRAYED);
    }
    else {

      EnableMenuItem((HMENU) wParam, MENU_SAVE_ID, MF_ENABLED);
    }
    return TRUE;
    break;

  case WM_CLOSE:

    DeleteObject(Hfont);
    DestroyMenu(hMenu);
    EndDialog(hwndDlg, 0);
    return TRUE;
    break;
  }

  return FALSE;
}

LRESULT WINAPI WindowProc(HWND hwndWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  DWORD threadId;
  int i;

  switch (uMsg) {

  case WM_SETCURSOR:
    SetCursor(LoadCursor(NULL, IDC_HAND));
    InvalidateRect(hwndWnd, NULL, TRUE);

    for (i = 0; i != TOTAL_ITEMS; ++i) {

      if (subclassed_items[i].itemHwnd == hwndWnd) {
	subclassed_items[i].current_color = active_color;
      }
    }

    return FALSE;
    break;

  case WM_LBUTTONDOWN:

    controlId = GetDlgCtrlID(hwndWnd);
    CreateThread(NULL, 0, ThreadFunc, GetParent(hwndWnd), 0, &threadId);
    break;
  }

  for (i = 0; i != TOTAL_ITEMS; ++i) {

    if (subclassed_items[i].itemHwnd == hwndWnd) {
      return CallWindowProc((WNDPROC) subclassed_items[i].oldWindowProc, hwndWnd, uMsg, wParam, lParam);
    }
  }
    
  return FALSE;
}

DWORD WINAPI AboutThreadFunc(LPVOID threadParam)
{
  DialogBoxParam(GetModuleHandle(0), aboutDlgName, (HWND) threadParam, AboutDialogProc, (LPARAM) NULL);

  return (DWORD) 0;
}

BOOL WINAPI AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  const int OK_BUTTON_ID = 0x302, BUILD_DATE_ID = 0x301, ICON_CTRL_ID = 0x303;
  char build_date[MAX_BUFFER_SIZE] = "Build date: ";

  switch (uMsg) {

  case WM_INITDIALOG:

    lstrcat(build_date, __DATE__);
    lstrcat(build_date, " - ");
    lstrcat(build_date, __TIME__);
    SendMessage(hwndDlg, WM_SETICON, (WPARAM) ICON_SMALL, (LPARAM) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(MATRIX_ICON_ID)));
    SetWindowText(GetDlgItem(hwndDlg, BUILD_DATE_ID), build_date);
    oldIconWndProc = SetWindowLong(GetDlgItem(hwndDlg, ICON_CTRL_ID), GWL_WNDPROC, (LONG) IconControlWndProc);
    break;

  case WM_COMMAND:

    if ((wParam & 0xFFFF) == OK_BUTTON_ID) {

      SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
    }
    break;

  case WM_CLOSE:

    if (NULL != playSoundAddr) {
      //PlaySound(NULL, NULL, 0);
      (*playSoundAddr)(NULL, NULL, 0);
    }

    EndDialog(hwndDlg, 0);
    break;
  }

  return FALSE;
}

LRESULT WINAPI IconControlWndProc(HWND hwndWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  const int SURPRISE_SOUND_ID = 0x304;
  static int click_count = 0;
  PAINTSTRUCT paint;
  HDC windowHdc = 0;

  switch (uMsg) {

  case WM_LBUTTONDOWN:

    ++click_count;

    if (click_count % 2) {

      if (NULL != playSoundAddr) {
	//PlaySound(MAKEINTRESOURCE(SURPRISE_SOUND_ID), GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
	(*playSoundAddr)(MAKEINTRESOURCE(SURPRISE_SOUND_ID), GetModuleHandle(0), SND_RESOURCE + SND_ASYNC);
      }
    }
    else {

      if (NULL != playSoundAddr) {
	//PlaySound(NULL, NULL, 0);
	(*playSoundAddr)(NULL, NULL, 0);
      }
    }

    break;

  case WM_PAINT:

    windowHdc = BeginPaint(hwndWnd, &paint);
    DrawIcon(windowHdc, 0, 0, LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(MATRIX_ICON_ID)));
    EndPaint(hwndWnd, &paint);
    break;
  }

  return CallWindowProc((WNDPROC) oldIconWndProc, hwndWnd, uMsg, wParam, lParam);

  return FALSE;
}

DWORD WINAPI ThreadFunc(LPVOID threadParam)
{
  DialogBoxParam(GetModuleHandle(0), inputDlgName, (HWND) threadParam, InputDialogProc, (LPARAM) NULL);

  return (DWORD) 0;
}

BOOL WINAPI InputDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  //  LOGFONT edit_font = { .lfHeight = 0, .lfWidth = 0, .lfEscapement = 0, .lfOrientation = 0, .lfWeight = FW_DONTCARE,
  //			.lfItalic = FALSE, .lfUnderline = FALSE, .lfStrikeOut = FALSE, .lfCharSet = DEFAULT_CHARSET,
  //			.lfOutPrecision = OUT_DEFAULT_PRECIS, .lfClipPrecision = CLIP_DEFAULT_PRECIS, .lfQuality = DEFAULT_QUALITY,
  //			.lfPitchAndFamily = DEFAULT_PITCH, .lfFaceName = "Courier New" };
  HANDLE hFile;
  static char *FileFilterStr[] = { "All files", "*.*", "Text files", "*.txt\0\0"};
  static char file_name_buffer[MAX_FILE_NAME];
  DWORD file_size = 0, bytes_read = 0;
  LPVOID file_buffer = NULL;
  OPENFILENAME ofn = { .lStructSize = sizeof(OPENFILENAME), .hwndOwner = hwndDlg, .lpstrFilter = FileFilterStr[0], .lpstrCustomFilter = NULL,
		       .nMaxCustFilter = 0, .nFilterIndex = 0, .lpstrFile = file_name_buffer, .nMaxFile =  MAX_FILE_NAME, 
		       .lpstrFileTitle = NULL, .nMaxFileTitle = 0, .lpstrInitialDir = NULL, .lpstrTitle = NULL, 
		       .Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, .nFileOffset = 0, .nFileExtension = 0,
		       .lpstrDefExt = NULL, .lCustData = 0, .lpfnHook = 0, .lpTemplateName = NULL };
  HFONT Hfont = (HFONT) NULL;
  char convert_buffer[MAX_BUFFER_SIZE];
  long double frobenius;
  matrix first_matrix, second_matrix, result_matrix;
  int i, first_buffer_size = 0, second_buffer_size = 0;
  LPVOID first_buffer, second_buffer;
  HMENU hMenu = 0;
  int caption_length = 0;
  const int append_size = 200;

  switch (uMsg) {

  case WM_CLOSE:
    DeleteObject(Hfont);
    EndDialog(hwndDlg, 0);
    return TRUE;
    break;

  case WM_INITDIALOG:

    hMenu = LoadMenu(GetModuleHandle(NULL), "BinaryMenu");
    SetMenu(hwndDlg, hMenu);
    caption_length = GetWindowTextLength(hwndDlg);
    first_buffer = VirtualAlloc(NULL, caption_length + append_size, MEM_COMMIT, PAGE_READWRITE);

    if (NULL != first_buffer) {

      GetWindowText(hwndDlg, first_buffer, caption_length + 1);

      switch (controlId) {

      case SYSTEM_ID:
	lstrcat(first_buffer, ": system of linear equations");
	break;

      case ADD_ID:
	lstrcat(first_buffer, ": matrix addition");
	break;

      case MUL_ID:
	lstrcat(first_buffer, ": matrix multiplication");
	break;

      case DIRECT_SUM_ID:
	lstrcat(first_buffer, ": direct sum");
	break;

      case KRONECKER_PRODUCT_ID:
	lstrcat(first_buffer, ": kronecker product");
	break;

      case HADAMARD_PRODUCT_ID:
	lstrcat(first_buffer, ": hadamard product");
	break;
	
      case KRONECKER_SUM_ID:
	lstrcat(first_buffer, ": kronecker sum");
	break;

      case FROBENIUS_PRODUCT_ID:
	lstrcat(first_buffer, ": frobenius product");
	break;
      }
      SetWindowText(hwndDlg, first_buffer);
      VirtualFree(first_buffer, caption_length + append_size, MEM_DECOMMIT);
    }


    //    Hfont = CreateFontIndirect(&edit_font);
    Hfont = (HFONT) SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), WM_GETFONT, (WPARAM) 0, (LPARAM) 0);
    SendMessage(GetDlgItem(hwndDlg, FIRST_EDIT_ID), WM_SETFONT, (WPARAM) Hfont, (LPARAM) TRUE);
    SendMessage(GetDlgItem(hwndDlg, SECOND_EDIT_ID), WM_SETFONT, (WPARAM) Hfont, (LPARAM) TRUE);

    for (i = 0; i != BINARY_DLG_TOTAL_ITEMS; ++i) {

      binary_subclassed_items[i].itemHwnd = GetDlgItem(hwndDlg,  binary_subclassed_items[i].itemId);
      binary_subclassed_items[i].oldWindowProc = SetWindowLong(GetDlgItem(hwndDlg,  binary_subclassed_items[i].itemId), 
							       GWL_WNDPROC, (LONG) binaryEditWindowProc);
    }

    SendMessage(GetDlgItem(hwndDlg, FIRST_EDIT_ID), WM_LBUTTONDOWN, (WPARAM) 0, (LPARAM) 0);
    SendMessage(hwndDlg, WM_SETICON, (WPARAM) ICON_SMALL, (LPARAM) LoadIcon(GetModuleHandle(0),
									    MAKEINTRESOURCE(MATRIX_ICON_ID)));
    break;

  case WM_LBUTTONDOWN:
    
    ReleaseCapture();
    SendMessage(hwndDlg, WM_SYSCOMMAND, (WPARAM) 61458, (LPARAM) 0);
    break;

  case WM_COMMAND:

    switch (wParam & 0xFFFF) {

    case MENU_OPEN_ID:

      for (i = 0; i != BINARY_DLG_TOTAL_ITEMS; ++i) {

	if (binary_subclassed_items[i].hasFocus) {
	  break;
	}
      }
      
      if (GetOpenFileName(&ofn)) {
      
	SendMessage(hwndDlg, WM_COMMAND, (WPARAM) SECOND_CLEAR_ID, (LPARAM) GetDlgItem(hwndDlg, SECOND_CLEAR_ID));
	hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE != hFile) {

	  file_size = GetFileSize(hFile, NULL);
	  file_buffer = VirtualAlloc(NULL, file_size, MEM_COMMIT, PAGE_READWRITE);

	  if (NULL == file_buffer) {
	    
	    CloseHandle(hFile);
	    MessageBox(hwndDlg, "Sorry, memory allocation failed.\r\nTry again!\n", NULL, MB_OK);
	  }
	  else {

	    ReadFile(hFile, file_buffer, file_size, &bytes_read, NULL);
	    SendMessage(binary_subclassed_items[i].itemHwnd, WM_SETTEXT, (WPARAM) 0, (LPARAM) file_buffer);
	    VirtualFree(file_buffer, file_size, MEM_DECOMMIT);
	    CloseHandle(hFile);
	  }
	}
	else {

	  MessageBox(hwndDlg, "Sorry, can't open your file.\r\nTry again!\n", NULL, MB_OK);
	}

	SetFocus(binary_subclassed_items[i].itemHwnd);
      }

      break;

    case MATRIX_CANCEL_ID:

      SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
      break;

    case SECOND_CLEAR_ID:

      for (i = 0; i != BINARY_DLG_TOTAL_ITEMS; ++i) {

	if (binary_subclassed_items[i].hasFocus) {
	  SetWindowText(GetDlgItem(hwndDlg, binary_subclassed_items[i].itemId), NULL);
	  break;
	}
      }

      break;

    case MATRIX_INPUT_ID:

      first_buffer_size = GetWindowTextLength(GetDlgItem(hwndDlg, FIRST_EDIT_ID));
      second_buffer_size = GetWindowTextLength(GetDlgItem(hwndDlg, SECOND_EDIT_ID));

      if (!first_buffer_size || !second_buffer_size) {
	return TRUE;
      }

      first_buffer = VirtualAlloc(NULL, first_buffer_size + 2, MEM_COMMIT, PAGE_READWRITE);
      second_buffer = VirtualAlloc(NULL, second_buffer_size + 2, MEM_COMMIT, PAGE_READWRITE);

      if (NULL == first_buffer) {

	MessageBox(hwndDlg, "Sorry, memory allocation failed. Try again.\n", NULL, MB_OK);

	if (NULL != second_buffer) {
	  VirtualFree(second_buffer, second_buffer_size + 2, MEM_DECOMMIT);
	}
	return TRUE;
      }
      else if (NULL == second_buffer) {

	MessageBox(hwndDlg, "Sorry, memory allocation failed. Try again.\n", NULL, MB_OK);
	VirtualFree(first_buffer, first_buffer_size + 2, MEM_DECOMMIT);
      
	return TRUE;
      }

      GetWindowText(GetDlgItem(hwndDlg, FIRST_EDIT_ID), first_buffer, first_buffer_size + 1);
      GetWindowText(GetDlgItem(hwndDlg, SECOND_EDIT_ID), second_buffer, second_buffer_size + 1);
      first_matrix = matrix_buffer_read(first_buffer);
      second_matrix = matrix_buffer_read(second_buffer);
      VirtualFree(first_buffer, first_buffer_size + 2, MEM_DECOMMIT);
      VirtualFree(second_buffer, second_buffer_size + 2, MEM_DECOMMIT);
      SetDlgItemText(hwndDlg, FIRST_EDIT_ID, NULL);
      SetDlgItemText(hwndDlg, SECOND_EDIT_ID, NULL);

      if (NULL == first_matrix.matrix_ptr) {

	if (NULL != second_matrix.matrix_ptr) {
	  matrix_free_memory(&second_matrix);
	}

	return TRUE;
      }
      else if (NULL == second_matrix.matrix_ptr) {
	matrix_free_memory(&first_matrix);
	return TRUE;
      }

      switch (controlId) {

      case FROBENIUS_PRODUCT_ID:

	frobenius = matrix_frobenius_product(first_matrix, second_matrix);

	if (!isnan(frobenius)) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nFrobenius product:\r\n\r\n");
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  snprintf(convert_buffer, sizeof(convert_buffer) - 1, "%Lf\r\n", frobenius);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) convert_buffer);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened.\r\nTry again!\n", NULL, MB_OK);
	}
	break;

      case KRONECKER_SUM_ID:

	result_matrix = matrix_kronecker_sum(first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nKronecker sum:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened.\r\nTry again!\n", NULL, MB_OK);
	}
	break;

      case SYSTEM_ID:

	result_matrix = solve_system_using_lup_decompozition(first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "Coefficient's matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nRight-hand side matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSystem solution:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened.\r\nProbably the system doesn't have a solution? Try again!\n", NULL, MB_OK);
	}
	break;

      case MUL_ID:

	result_matrix = matrix_multiplication(first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nMultiplication result:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened. Probably the matrices aren't multiplicable?\r\nTry again!\n", NULL, MB_OK);
	}
	break;

      case ADD_ID:

	result_matrix = matrix_addition(first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nAddition result:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened. Probably the matrices don't have right dimensions?\r\nTry again!\n", NULL, MB_OK);
	}
	break;

      case KRONECKER_PRODUCT_ID:

	result_matrix = matrix_kronecker_product(first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nKronecker product:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened. Try again!\n", NULL, MB_OK);
	}
	break;

      case DIRECT_SUM_ID:

	result_matrix = matrix_direct_sum(2, first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nDirect sum:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened. Try again!\n", NULL, MB_OK);
	}
	break;

      case HADAMARD_PRODUCT_ID:

	result_matrix = matrix_hadamard_product(first_matrix, second_matrix);

	if (NULL != result_matrix.matrix_ptr) {

	  SendMessage(mainHwnd, WM_COMMAND, (WPARAM) MATRIX_CLEAR_ID, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "First matrix:\r\n\r\n");
	  matrix_buffer_print(first_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nSecond matrix:\r\n\r\n");
	  matrix_buffer_print(second_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_SETSEL, (WPARAM) -1, (LPARAM) 0);
	  SendMessage(GetDlgItem(mainHwnd, MATRIX_EDIT_ID), EM_REPLACESEL, (WPARAM) NULL, (LPARAM) "\r\nHadamard product:\r\n\r\n");
	  matrix_buffer_print(result_matrix, GetDlgItem(mainHwnd, MATRIX_EDIT_ID));
	  matrix_free_memory(&result_matrix);
	  SendMessage(hwndDlg, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
	}
	else {

	  MessageBox(hwndDlg, "Sorry, something bad happened. Try again!\n", NULL, MB_OK);
	}
	break;
      }
 
      matrix_free_memory(&first_matrix);
      matrix_free_memory(&second_matrix);
      break;
    }

    break;
  }

  return FALSE;
}

LRESULT WINAPI binaryEditWindowProc(HWND hwndWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int i;

  switch (uMsg) {

  case WM_LBUTTONDOWN:

    for (i = 0; i != BINARY_DLG_TOTAL_ITEMS; ++i) {

      if (binary_subclassed_items[i].itemHwnd == hwndWnd) {
	binary_subclassed_items[i].hasFocus = true;
      }
      else {
	binary_subclassed_items[i].hasFocus = false;
      }
    }
    break;
  }

  for (i = 0; i != BINARY_DLG_TOTAL_ITEMS; ++i) {

    if (binary_subclassed_items[i].itemHwnd == hwndWnd) {
      return CallWindowProc((WNDPROC) binary_subclassed_items[i].oldWindowProc, hwndWnd, uMsg, wParam, lParam);
    }
  }
    

  return FALSE;
}
