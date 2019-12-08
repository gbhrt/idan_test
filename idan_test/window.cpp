#include <windows.h>
#include<iostream>
#define ID_EDIT 1
#define ID_BUTTON 2
#include "Header.h"

using namespace std;


//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	PWSTR lpCmdLine, int nCmdShow) {
wchar_t text[100];
int len = 0;
HWND hwnd;

char internet_ip[20];
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int get_command()
{

	HINSTANCE hInstance = 0;
	MSG  msg;

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"insert IP window";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	hwnd = CreateWindowW(wc.lpszClassName, L"Edit control",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		220, 220, 280, 200, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {

	static HWND hwndEdit;
	HWND hwndButton;

	switch (msg) {

	case WM_CREATE:

		hwndEdit = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			50, 50, 150, 20, hwnd, (HMENU)ID_EDIT,
			NULL, NULL);

		hwndButton = CreateWindowW(L"button", L"Set command",
			WS_VISIBLE | WS_CHILD, 50, 100, 80, 25,
			hwnd, (HMENU)ID_BUTTON, NULL, NULL);

		break;

	case WM_COMMAND:

		if (HIWORD(wParam) == BN_CLICKED) {

			len = GetWindowTextLengthW(hwndEdit) + 1;


			GetWindowTextW(hwndEdit, text, len);
			char massage[10];
			for (int i = 0; i < len; i++)
				massage[i] = text[i];
			acc = atof(massage);

			/*	for(int i = 0; i<len; i++)
			cout << massage[i];
			cout << endl;*/
			/*char dd[] = { "abc" };
			cout << dd[0];*/
			//SetWindowTextW(hwnd, text);
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}


