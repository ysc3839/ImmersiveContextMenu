#pragma once

HRESULT DrawThemeTextWithFontWeight(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, LONG fontWeight)
{
	DTTOPTS opts = {
		.dwSize = sizeof(opts),
		.dwFlags = DTT_COMPOSITED | DTT_CALLBACK,
		.pfnDrawTextCallback = [](HDC hdc, LPWSTR pszText, int cchText, LPRECT prc, UINT dwFlags, LPARAM lParam) -> int {
			auto hCurrentFont = GetCurrentObject(hdc, OBJ_FONT);
			if (!hCurrentFont)
				return 0;

			LOGFONTW font;
			if (GetObjectW(hCurrentFont, sizeof(font), &font) <= 0)
				return 0;

			font.lfWeight = static_cast<LONG>(lParam);
			wil::unique_hfont hFont(CreateFontIndirectW(&font));
			if (!hFont)
				return 0;

			auto select = wil::SelectObject(hdc, hFont.get());
			return DrawTextExW(hdc, pszText, cchText, prc, dwFlags, nullptr);
		},
		.lParam = static_cast<LPARAM>(fontWeight)
	};
	return DrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, &opts);
}

HRESULT DrawThemeTextWithFontWeight(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, std::wstring_view text, DWORD dwTextFlags, LPRECT pRect, LONG fontWeight)
{
	return DrawThemeTextWithFontWeight(hTheme, hdc, iPartId, iStateId, text.data(), static_cast<int>(text.size()), dwTextFlags, pRect, fontWeight);
}

HTHEME OpenThemeDataForDpiWithFallback(HWND hWnd, LPCWSTR pszClassList, UINT dpi)
{
	static bool checked = false;
	static decltype(OpenThemeDataForDpi)* _OpenThemeDataForDpi = nullptr;
	if (!checked)
	{
		_OpenThemeDataForDpi = GetProcAddressByFunctionDeclaration(GetModuleHandleW(L"uxtheme.dll"), OpenThemeDataForDpi);
		checked = true;
	}

	if (_OpenThemeDataForDpi)
		return _OpenThemeDataForDpi(hWnd, pszClassList, dpi);
	return OpenThemeData(hWnd, pszClassList);
}
