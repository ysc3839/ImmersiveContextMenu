#include "pch.h"
#include "ImmersiveContextMenu.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ [[maybe_unused]] HINSTANCE hPrevInstance,
	_In_ [[maybe_unused]] LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	g_hInst = hInstance;

	WNDCLASSEXW wcex = {
		.cbSize = sizeof(wcex),
		.lpfnWndProc = WndProc,
		.hInstance = hInstance,
		.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_IMMERSIVECONTEXTMENU)),
		.hCursor = LoadCursorW(nullptr, IDC_ARROW),
		.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
		.lpszMenuName = MAKEINTRESOURCEW(IDC_IMMERSIVECONTEXTMENU),
		.lpszClassName = L"ImmersiveContextMenu",
		.hIconSm = wcex.hIcon
	};

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(L"ImmersiveContextMenu", L"ImmersiveContextMenu", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return EXIT_FAILURE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
	//case WM_INITMENUPOPUP:
		if (ImmersiveContextMenu::ContextMenuWndProc(hWnd, message, wParam, lParam))
			return 0;
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_CREATE:
	{
		auto hMenu = GetMenu(hWnd);
		CheckMenuRadioItem(hMenu, ID_VIEW_MEDIUMICONS, ID_VIEW_MEDIUMICONS, ID_VIEW_MEDIUMICONS, MF_BYCOMMAND);
		SetMenuDefaultItem(GetSubMenu(hMenu, 2), IDM_ABOUT, FALSE);

		auto cmrdArray = new CMRDArray;
		ImmersiveContextMenu::ApplyOwnerDrawToMenu(hMenu, hWnd, nullptr, ICMO_USEPPI | ICMO_FORCEMOUSESTYLING | ICMO_MENUBAR, cmrdArray);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBoxW(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
