#include "stdafx.h"
#include "task1WINAPI.h"
#include "CommCtrl.h"
#include "Winuser.h"


#define MAX_LOADSTRING 100

// Глобальные переменные:
BOOL OPEN = false;
HINSTANCE hInst;                                //текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  //Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hwndbank[10];								//хранилище hwnd
TCHAR Buff[100];								//текстовый буфер
DWORD BSize = 100;								//размер буфера
HANDLE handlebank[10];							//хранилище handle'ов
OPENFILENAME ofn;								//структура для окон открытия и сохранения
TCHAR PATH[100];								//путь файла
TCHAR NAME[100];								//имя файла
HWND hWndMain;									//hwnd главного окна
TCHAR Name[] = L"Text Editor";					//имя главного окна
HKEY hKey;
int wl, wh;


// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	
    //code here
	RegCreateKeyEx(HKEY_CURRENT_USER, L"txted", 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, 0);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TASK1WINAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASK1WINAPI));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(hWndMain/*msg.hwnd*/, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASK1WINAPI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TASK1WINAPI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

HWND CreateToolbar(HWND hWndParent)
{
	HIMAGELIST g_hImageList = NULL;

	const int ImageListID = 0;
	const int numButtons = 7;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_FLAT, 0, 0, 0, 0,
		hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Open" },
		{ MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Save" },
		{ MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVEAS, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Save as" },
		{ MAKELONG(STD_UNDO,	 ImageListID), IDM_UNDO, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Undo" },
		{ MAKELONG(STD_CUT,      ImageListID), IDM_CUT,  TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Cut" },
		{ MAKELONG(STD_COPY,     ImageListID), IDM_COPY, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Copy" },
		{ MAKELONG(STD_PASTE,    ImageListID), IDM_PASTE,TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Paste" }
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}

HWND CreateEditControl(HWND hWndParent) 
{
	HWND editControl = CreateWindowEx(
		0, L"EDIT",
		0,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER | WS_CLIPSIBLINGS,
		0, 0, 0, 0,   // set size in WM_SIZE message 
		hWndParent,         // parent window 
		(HMENU)ID_EDITCHILD,   // edit control ID 
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE),
		NULL);

	return editControl;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	InitCommonControls();

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)Name);

   hWndMain = hWnd;

   HWND hPathText = CreateWindowEx(0, L"STATIC", L"Path to the file:",
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_SIMPLE | WS_CLIPSIBLINGS,
	   0, 0, 0, 0,
	   hWnd, 0, hInst, NULL);
   HWND hPath = CreateWindowEx(0, L"STATIC", PATH,
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_SIMPLE | WS_CLIPSIBLINGS,
	   0, 0, 0, 0,
	   hWnd, 0, hInst, NULL);

   HWND saveB = CreateWindowEx(0, L"BUTTON", L"save",
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_CLIPSIBLINGS,
	   0, 0, 0, 0,
	   hWnd, (HMENU)IDM_SAVE, hInst, NULL);
   HWND openB = CreateWindowEx(0, L"BUTTON", L"open",
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_CLIPSIBLINGS,
	   0, 0, 0, 0,
	   hWnd, (HMENU)IDM_OPEN, hInst, NULL);
   HWND saveasB = CreateWindowEx(0, L"BUTTON", L"save as",
	   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_CLIPSIBLINGS,
	   0, 0, 0, 0,
	   hWnd, (HMENU)IDM_SAVEAS, hInst, NULL);

   hwndbank[0] = CreateToolbar(hWnd);
   hwndbank[1] = CreateEditControl(hWnd);
   hwndbank[2] = saveB;
   hwndbank[3] = openB;
   hwndbank[4] = saveasB;
   hwndbank[5] = hPathText;
   hwndbank[6] = hPath;

   DWORD t1 = 0, t2 = sizeof(wl), t3 = 0, t4 = sizeof(wh);
   RegOpenKeyEx(HKEY_CURRENT_USER, L"txted", 0, KEY_ALL_ACCESS, &hKey);
   RegQueryValueExA(hKey, (LPCSTR)L"l", 0, (LPDWORD)& t1, (BYTE*)& wl, &t2); 
   RegQueryValueExA(hKey, (LPCSTR)L"h", 0, (LPDWORD)& t3, (BYTE*)& wh, &t4); 

   SetWindowPos(hWnd, 0, 0, 0, wl, wh, SWP_NOMOVE);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

			case IDM_SAVE: 
			{
				if (!OPEN) {
					SendMessage(hwndbank[4], BM_CLICK, 0, 0);
				}
				else {
					CloseHandle(handlebank[0]);
					SendMessage(hwndbank[1], WM_GETTEXT, 100, (LPARAM)Buff);
					handlebank[0] = CreateFile(PATH,
						GENERIC_READ | GENERIC_WRITE,
						0,
						(LPSECURITY_ATTRIBUTES)NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						(HANDLE)NULL);
					WriteFile(handlebank[0], &Buff, sizeof(Buff), &BSize, NULL);
				}
			}
				break;

			case IDM_SAVEAS:
			{
				HANDLE hl;
				OPENFILENAME sfn;
				char szSaveFileName[MAX_PATH];
				ZeroMemory(&sfn, sizeof(sfn));
				sfn.lStructSize = sizeof(sfn);
				sfn.hwndOwner = hWnd;
				sfn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0";
				sfn.lpstrFile = PATH;
				sfn.lpstrFileTitle = NAME;
				sfn.nMaxFileTitle = sizeof(NAME);
				sfn.nMaxFile = MAX_PATH;
				sfn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				sfn.lpstrDefExt = L".txt";
				if (GetSaveFileName(&sfn)) {
					hl = CreateFile(sfn.lpstrFile,
						GENERIC_READ | GENERIC_WRITE,
						0,
						(LPSECURITY_ATTRIBUTES)NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						(HANDLE)NULL);
					handlebank[0] = hl;
					SendMessage(hwndbank[1], WM_GETTEXT, 100, (LPARAM)Buff);
					WriteFile(handlebank[0], &Buff, sizeof(Buff), &BSize, NULL);
					CloseHandle(handlebank[0]);
				}
			}
				break;

			case IDM_UNDO:
				if (SendMessage(hwndbank[1], EM_CANUNDO, 0, 0))
					SendMessage(hwndbank[1], WM_UNDO, 0, 0);
				else
				{
				 //как изменить стиль на неработающий??
				}
				break;

			case IDM_CUT:
				SendMessage(hwndbank[1], WM_CUT, 0, 0);
				break;

			case IDM_COPY:
				SendMessage(hwndbank[1], WM_COPY, 0, 0);
				break;

			case IDM_PASTE:
				SendMessage(hwndbank[1], WM_PASTE, 0, 0);
				break;

			case IDM_NEW:
			{

			}
			break;

			case IDM_OPEN:
			{
				OPEN = true;
				HANDLE hl;
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = PATH;
				ofn.nMaxFile = sizeof(PATH);
				ofn.lpstrFileTitle = NAME;
				ofn.nMaxFileTitle = sizeof(NAME);
				ofn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0";
				ofn.nFilterIndex = 2;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				//
				if (GetOpenFileName(&ofn) == TRUE) {
					hl = CreateFile(ofn.lpstrFile,
						GENERIC_READ | GENERIC_WRITE,
						0,
						(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						(HANDLE)NULL);
					handlebank[0] = hl;
				}
				ReadFile(handlebank[0], &Buff, sizeof(Buff), &BSize, NULL);
				SendMessage(hwndbank[1], WM_SETTEXT, 0, (LPARAM)Buff);
				SendMessage(hwndbank[6], WM_SETTEXT, 0, (LPARAM)PATH);
			}
			break;

			break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:

		RegSetValueExA(hKey, (LPCSTR)L"l", 0, REG_DWORD, (const BYTE*)& wl, sizeof(wl));
		RegSetValueExA(hKey, (LPCSTR)L"h", 0, REG_DWORD, (const BYTE*)& wh, sizeof(wh));
		RegCloseKey(hKey);
		PostQuitMessage(0);

        break;
	case WM_CLOSE:
		if (SendMessage(hwndbank[1], WM_GETTEXT, 100, (LPARAM)Buff)) {
			Buff;
			switch (int a = MessageBox(hWnd, L"Хотите сохранить изменения?", L"Выход", MB_YESNOCANCEL | MB_ICONQUESTION)) 
			{
			case IDYES:
				SendMessage(hwndbank[2], BM_CLICK, 0, 0);
				DestroyWindow(hWnd);
				PostQuitMessage(0);
				break;
			case IDNO:
				DestroyWindow(hWnd);
				PostQuitMessage(0);
				break;
			}
		}
		else {
			DestroyWindow(hWnd);
			PostQuitMessage(0);
		}
		break;
	case WM_SIZE:
	{
		SetWindowPos(hwndbank[0], 0, 0, 0, 0, 0, 0);
		SetWindowPos(hwndbank[1], 0, 10, 45, LOWORD(lParam) - 10, HIWORD(lParam) - 100, 0);
		SetWindowPos(hwndbank[2], 0, LOWORD(lParam) - 132, HIWORD(lParam) - 40, 60, 30, 0);
		SetWindowPos(hwndbank[3], 0, LOWORD(lParam) - 194, HIWORD(lParam) - 40, 60, 30, 0);
		SetWindowPos(hwndbank[4], 0, LOWORD(lParam) - 70, HIWORD(lParam) - 40, 60, 30, 0);
		SetWindowPos(hwndbank[5], 0, 10, HIWORD(lParam) - 40, LOWORD(lParam) - 220, 20, 0);
		SetWindowPos(hwndbank[6], 0, 110, HIWORD(lParam) - 40, 150, 20, 0);


		wl = LOWORD(lParam);
		wh = HIWORD(lParam);
	}
		break;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 600;
		lpMMI->ptMinTrackSize.y = 420;
	}
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
