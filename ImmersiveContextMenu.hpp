#pragma once
#include "UxThemeHelper.hpp"

#ifndef ICM_NOSTOREMENUDATA
#define ICM_NOSTOREMENUDATA 1
#endif

#define MOUSEEVENTF_FROMTOUCH_NOPEN 0xFF515780

bool RegGetBoolWithFlags(HKEY hkey, const wchar_t* subKey, const wchar_t* value, DWORD flags = 0) noexcept
{
	uint32_t data;
	DWORD type = 0, cbData = sizeof(data);
	auto err = RegGetValueW(hkey, subKey, value, flags | RRF_RT_REG_DWORD | RRF_RT_REG_SZ | RRF_NOEXPAND, &type, &data, &cbData);
	if (err == ERROR_SUCCESS)
	{
		if (type == REG_DWORD)
		{
			return data == 1;
		}
		else if (cbData == sizeof(L"1") && data - L'0' <= 1)
		{
			return static_cast<wchar_t>(data) == L'1';
		}
	}
	return false;
}

enum ScaleType
{
	DPI,
	PPI // This name is from debug symbol. Actually it means scale by window dpi, I don't know why it's called PPI.
};

int ScaleBySpecificDPI(int i, int dpi)
{
	return MulDiv(i, dpi, USER_DEFAULT_SCREEN_DPI);
}

int ScaleByPPI(int i, HWND hWnd)
{
	return MulDiv(i, GetDpiForWindow(hWnd), USER_DEFAULT_SCREEN_DPI);
}

int ScaleByType(ScaleType type, int i, HWND hWnd, int dpi)
{
	if (type == PPI)
		return ScaleByPPI(i, hWnd);
	else
		return ScaleBySpecificDPI(i, dpi);
}

enum ImmersiveContextMenuOptions
{
	ICMO_USEPPI = 0x1,
	ICMO_OVERRIDECOMPATCHECK = 0x2,
	ICMO_FORCEMOUSESTYLING = 0x4,
	ICMO_USESYSTEMTHEME = 0x8,
	ICMO_ICMBRUSHAPPLIED = 0x10,
	ICMO_MENUBAR = 0x20
};
DEFINE_ENUM_FLAG_OPERATORS(ImmersiveContextMenuOptions);

enum ContextMenuPaddingType
{
	CMPT_NONE = 0x0,
	CMPT_TOP_PADDING = 0x1,
	CMPT_BOTTOM_PADDING = 0x2,
	CMPT_TOUCH_INPUT = 0x4,
};
DEFINE_ENUM_FLAG_OPERATORS(ContextMenuPaddingType);

struct ContextMenuRenderingData;
using CMRDArray = std::vector<std::unique_ptr<ContextMenuRenderingData>>;

#define SUBMENU 0x10 // For menuFlags
#define MENUBAR 0x1 // For menuFlags
struct ContextMenuRenderingData
{
	std::wstring text;
	UINT menuFlags;
	HBITMAP hbmpItem;
	/*HBITMAP hbmpChecked;
	HBITMAP hbmpUnchecked;*/
	ContextMenuPaddingType cmpt;
	ScaleType scaleType;
	UINT dpi;
	bool useDarkTheme;
	bool useSystemPadding;
	bool forceAccelerators;
	CMRDArray* parentArray;
};

namespace ContextMenuDefinitions
{
	constexpr UINT separatorVerticalPixels = 7;
	constexpr UINT separatorVerticalPixelsWithPadding = 9;
	constexpr UINT separatorHorizontalPaddingPixels = 8;
	constexpr UINT separatorBackgroundVerticalPixels = 5;
	constexpr UINT textVerticalPixelsToFix = 13;
	constexpr UINT nonTextHorizontalPixels = 93;
	constexpr UINT nonTextVerticalPixels = 14;
	constexpr UINT nonTextVerticalPixelsWithPadding = 24;
	constexpr UINT nonTextVerticalPixelsTouchInput = 36;
	constexpr UINT topPaddingPixels = 2;
	constexpr UINT topPaddingPixelsWithPaddingOrTouch = 6;
	constexpr UINT bottomPaddingPixels = 2;
	constexpr UINT bottomPaddingPixelsWithPaddingOrTouch = 6;
	constexpr UINT iconSize = 16;
	constexpr UINT iconHorizontalPadding = 8;
	constexpr UINT totalIconHorizontalPixels = iconHorizontalPadding + iconSize + iconHorizontalPadding;
	constexpr UINT rightPaddingPixels = 4;
	constexpr UINT textRightPaddingPixels = 17;
}

namespace ImmersiveContextMenu
{
	HRESULT ApplyOwnerDrawToMenu(HMENU hMenu, HWND hWnd, LPPOINT point, ImmersiveContextMenuOptions icmoFlags, CMRDArray* cmrdArray);
	bool CanApplyOwnerDrawToMenu(HMENU hMenu, HWND hWnd) noexcept;
	ContextMenuRenderingData* GetContextMenuDataForItem([[maybe_unused]] HWND hWnd, ULONG_PTR itemData, [[maybe_unused]] UINT itemID) noexcept;
	wil::unique_htheme GetCurrentThemeForOverrides(HWND hWnd, ImmersiveContextMenuOptions icmoFlags, bool useDarkTheme) noexcept;
	void HandleMergedMenus(HMENU hMenu, HWND hWnd);
	HRESULT OverrideBackgroundColor(HMENU hMenu, HWND hWnd, ImmersiveContextMenuOptions icmoFlags) noexcept;
	void RemoveContextMenuDataForItem([[maybe_unused]] HWND hWnd, [[maybe_unused]] ULONG_PTR itemData, [[maybe_unused]] UINT itemID) noexcept;
	void RemoveOwnerDrawFromMenu(HMENU hMenu, HWND hWnd) noexcept;
	bool ShouldUseDarkTheme(ImmersiveContextMenuOptions icmoFlags) noexcept;
	bool StoreContextMenuDataForItem([[maybe_unused]] HWND hWnd, [[maybe_unused]] ULONG_PTR itemData, [[maybe_unused]] UINT itemID, [[maybe_unused]] ContextMenuRenderingData* cmrd) noexcept;
	UINT _GetDpiForMonitorFromPoint(LPPOINT point) noexcept;
	HRESULT _GetRenderingDataForMenuItem(ScaleType type, LPMENUITEMINFOW mii, std::wstring&& itemText, LPPOINT point, CMRDArray* cmrdArray, ContextMenuRenderingData* parentCmrd, ImmersiveContextMenuOptions icmoFlags, ContextMenuRenderingData** cmrd) noexcept;
	void _RemoveOwnerDrawFromMenuWorker(HMENU hMenu, HWND hWnd) noexcept;
	void _RemoveParentArrayFromWindow(HWND hWnd) noexcept;
	CMRDArray* _RetrieveParentArrayFromWindow(HWND hWnd) noexcept;
	bool _StoreParentArrayOnWindow(HWND hWnd, CMRDArray* cmrdArray) noexcept;

	static bool shouldUseDarkTheme = false;

	HRESULT ApplyOwnerDrawToMenu(HMENU hMenu, HWND hWnd, LPPOINT point, ImmersiveContextMenuOptions icmoFlags, CMRDArray* cmrdArray)
	{
		if (WI_IsFlagClear(icmoFlags, ICMO_OVERRIDECOMPATCHECK))
		{
			if (!CanApplyOwnerDrawToMenu(hMenu, hWnd))
				return S_FALSE;
		}
		bool touchInput = false;
		if (WI_IsFlagClear(icmoFlags, ICMO_FORCEMOUSESTYLING))
		{
			INPUT_MESSAGE_SOURCE ims;
			if (GetCurrentInputMessageSource(&ims) && ims.deviceType == IMDT_TOUCH ||
				WI_AreAllFlagsSet(GetMessageExtraInfo(), MOUSEEVENTF_FROMTOUCH_NOPEN))
				touchInput = true;
		}

		HRESULT hr = E_FAIL;
		if (_StoreParentArrayOnWindow(hWnd, cmrdArray))
			hr = S_OK;

		shouldUseDarkTheme = ShouldUseDarkTheme(icmoFlags);

		BOOL forceAccelerators = FALSE;
		SystemParametersInfoW(SPI_GETMENUUNDERLINES, 0, &forceAccelerators, 0);

		ContextMenuRenderingData* renderData = nullptr;
		for (UINT i = 0; SUCCEEDED(hr); ++i)
		{
			std::wstring buf(64, L'\0');
			MENUITEMINFOW mii = {
				.cbSize = sizeof(mii),
				.fMask = MIIM_FTYPE | MIIM_BITMAP | MIIM_STRING | MIIM_DATA | MIIM_CHECKMARKS | MIIM_SUBMENU | MIIM_ID | MIIM_STATE,
				.dwTypeData = buf.data(),
				.cch = static_cast<UINT>(buf.size())
			};
			if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				continue;
			}

			hr = S_OK;
			buf.resize(static_cast<size_t>(mii.cch));
			buf.shrink_to_fit();

			auto parentCmrd = GetContextMenuDataForItem(hWnd, mii.dwItemData, mii.wID);
			renderData = parentCmrd;
			if ((WI_IsFlagClear(mii.fType, MFT_OWNERDRAW) || !mii.dwItemData)
				&& (!parentCmrd || touchInput != WI_IsFlagSet(parentCmrd->cmpt, CMPT_TOUCH_INPUT)))
			{
				ContextMenuRenderingData* newRenderData;
				hr = _GetRenderingDataForMenuItem(WI_IsFlagSet(icmoFlags, ICMO_USEPPI) ? PPI : DPI, &mii, std::move(buf), point, cmrdArray, parentCmrd, icmoFlags, &newRenderData);
				if (SUCCEEDED(hr))
				{
					renderData = newRenderData;
					renderData->forceAccelerators = forceAccelerators;
					cmrdArray->emplace_back(renderData);

					mii.fMask = MIIM_FTYPE | MIIM_BITMAP | MIIM_CHECKMARKS;
					WI_SetFlag(mii.fType, MFT_OWNERDRAW);
					mii.hbmpItem = nullptr;
					if (!mii.dwItemData)
					{
						WI_SetFlag(mii.fMask, MIIM_DATA);
						mii.dwItemData = reinterpret_cast<ULONG_PTR>(renderData);
					}
					if (StoreContextMenuDataForItem(hWnd, mii.dwItemData, mii.wID, renderData))
					{
						if (SetMenuItemInfoW(hMenu, i, TRUE, &mii))
						{
							hr = S_OK;
							if (mii.hSubMenu)
							{
								if (WI_IsFlagSet(icmoFlags, ICMO_OVERRIDECOMPATCHECK))
									HandleMergedMenus(mii.hSubMenu, hWnd);
								else
									ApplyOwnerDrawToMenu(mii.hSubMenu, hWnd, point, (icmoFlags & ~ICMO_MENUBAR) | ICMO_ICMBRUSHAPPLIED, cmrdArray);
							}
						}
					}
					renderData->parentArray = cmrdArray;
					WI_SetFlagIf(renderData->cmpt, CMPT_TOUCH_INPUT, touchInput);
				}
			}
			if (renderData && WI_IsFlagClear(icmoFlags, ICMO_MENUBAR))
			{
				if (i != 0)
					WI_ClearFlag(renderData->cmpt, CMPT_TOP_PADDING);
				else
					WI_SetFlag(renderData->cmpt, CMPT_TOP_PADDING);
				WI_ClearFlag(renderData->cmpt, CMPT_BOTTOM_PADDING);
			}
		}
		if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_MENU_ITEM_NOT_FOUND))
		{
			RemoveOwnerDrawFromMenu(hMenu, hWnd);
			return hr;
		}
		hr = S_OK;
		if (renderData && WI_IsFlagClear(icmoFlags, ICMO_MENUBAR))
			WI_SetFlag(renderData->cmpt, CMPT_BOTTOM_PADDING);
		if (WI_IsFlagClear(icmoFlags, ICMO_ICMBRUSHAPPLIED))
			OverrideBackgroundColor(hMenu, hWnd, icmoFlags);
		return hr;
	}

	bool CanApplyOwnerDrawToMenu(HMENU hMenu, HWND hWnd) noexcept
	{
		MENUITEMINFOW mii;
		for (UINT i = 0; ; ++i)
		{
			mii = {
				.cbSize = sizeof(mii),
				.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID
			};
			if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii))
				break;
			if (WI_IsFlagSet(mii.fType, MFT_OWNERDRAW) &&
				!(mii.dwItemData && GetContextMenuDataForItem(hWnd, mii.dwItemData, mii.wID)))
				return false;
		}
		return true;
	}

	ContextMenuRenderingData* GetContextMenuDataForItem([[maybe_unused]] HWND hWnd, ULONG_PTR itemData, [[maybe_unused]] UINT itemID) noexcept
	{
#if ICM_NOSTOREMENUDATA
		return reinterpret_cast<ContextMenuRenderingData*>(itemData);
#else
		wchar_t propName[64]; // ImmersiveContextMenuArray_FFFFFFFFFFFFFFFF-4294967295
		if (swprintf_s(propName, L"ImmersiveContextMenuArray_%" PRIXPTR "-%lu", itemData, itemID) != -1)
			return reinterpret_cast<ContextMenuRenderingData*>(GetPropW(hWnd, propName));
		return nullptr;
#endif
	}

	wil::unique_htheme GetCurrentThemeForOverrides(HWND hWnd, ImmersiveContextMenuOptions icmoFlags, bool useDarkTheme) noexcept
	{
		const wchar_t* const lightClassName = WI_IsFlagSet(icmoFlags, ICMO_USESYSTEMTHEME) ? L"LightMode_ImmersiveStart::Menu" : L"ImmersiveStart::Menu";
		const wchar_t* const className = useDarkTheme ? L"DarkMode_ImmersiveStart::Menu" : lightClassName;
		HTHEME hTheme = OpenThemeData(hWnd, className);
		if (!hTheme)
			hTheme = OpenThemeData(GetParent(hWnd), className);
		return wil::unique_htheme(hTheme);
	}

	void HandleMergedMenus(HMENU hMenu, HWND hWnd)
	{
		bool hasOwnerDraw = true;
		bool itemDataPresent = true;
		ContextMenuRenderingData* renderData = nullptr;
		for (UINT i = 0; ; ++i)
		{
			MENUITEMINFOW mii = {
				.cbSize = sizeof(mii),
				.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID
			};
			if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii))
				break;
			if (WI_IsFlagClear(mii.fType, MFT_OWNERDRAW))
			{
				hasOwnerDraw = false;
				continue;
			}
			if (mii.dwItemData)
			{
				auto data = GetContextMenuDataForItem(hWnd, mii.dwItemData, mii.wID);
				if (data)
					renderData = data;
				else
					hasOwnerDraw = false;
			}
			else
				itemDataPresent = false;
		}

		if (!hasOwnerDraw && itemDataPresent)
		{
			auto cmrdArray = renderData ? renderData->parentArray : nullptr;
			if (!cmrdArray)
				cmrdArray = _RetrieveParentArrayFromWindow(hWnd);
			if (cmrdArray)
			{
				bool msgExtraInfoOverrided = false;
				LPARAM oldMsgExtraInfo = 0;
				if (!cmrdArray->empty() && WI_IsFlagSet(cmrdArray->front()->cmpt, CMPT_TOUCH_INPUT))
				{
					msgExtraInfoOverrided = true;
					oldMsgExtraInfo = SetMessageExtraInfo(MOUSEEVENTF_FROMTOUCH_NOPEN);
				}

				ImmersiveContextMenuOptions icmo = ICMO_ICMBRUSHAPPLIED | ICMO_OVERRIDECOMPATCHECK;
				if (renderData)
				{
					WI_SetFlagIf(icmo, ICMO_USESYSTEMTHEME, renderData->useSystemPadding);
				}

				ApplyOwnerDrawToMenu(hMenu, hWnd, nullptr, icmo, cmrdArray);

				if (msgExtraInfoOverrided)
					SetMessageExtraInfo(oldMsgExtraInfo);
			}
		}
	}

	HRESULT OverrideBackgroundColor(HMENU hMenu, HWND hWnd, ImmersiveContextMenuOptions icmoFlags) noexcept
	{
		auto hTheme = GetCurrentThemeForOverrides(hWnd, icmoFlags, shouldUseDarkTheme);
		RETURN_HR_IF_NULL(E_UNEXPECTED, hTheme);

		COLORREF color;
		RETURN_IF_FAILED(GetThemeColor(hTheme.get(), MENU_POPUPBACKGROUND, 0, TMT_FILLCOLOR, &color));

		MENUINFO mi = {
			.cbSize = sizeof(mi),
			.fMask = MIM_APPLYTOSUBMENUS | MIM_BACKGROUND,
			.hbrBack = CreateSolidBrush(color)
		};
		RETURN_IF_WIN32_BOOL_FALSE(SetMenuInfo(hMenu, &mi));

		return S_OK;
	}

	void RemoveContextMenuDataForItem([[maybe_unused]] HWND hWnd, [[maybe_unused]] ULONG_PTR itemData, [[maybe_unused]] UINT itemID) noexcept
	{
#if !ICM_NOSTOREMENUDATA
		wchar_t propName[64]; // ImmersiveContextMenuArray_FFFFFFFFFFFFFFFF-4294967295
		if (swprintf_s(propName, L"ImmersiveContextMenuArray_%" PRIXPTR "-%lu", itemData, itemID) != -1)
			RemovePropW(hWnd, propName);
#endif
	}

	void RemoveOwnerDrawFromMenu(HMENU hMenu, HWND hWnd) noexcept
	{
		_RemoveOwnerDrawFromMenuWorker(hMenu, hWnd);

		MENUINFO mi = {
			.cbSize = sizeof(mi),
			.fMask = MIM_BACKGROUND
		};
		if (GetMenuInfo(hMenu, &mi))
		{
			if (mi.hbrBack)
			{
				DeleteObject(mi.hbrBack);
				mi.hbrBack = nullptr;
				SetMenuInfo(hMenu, &mi);
			}
		}

		_RemoveParentArrayFromWindow(hWnd);
	}

	bool ShouldUseDarkTheme([[maybe_unused]] ImmersiveContextMenuOptions icmoFlags) noexcept
	{
		/*if (WI_IsFlagClear(icmoFlags, ICMO_USESYSTEMTHEME))
			return false;*/
		return !RegGetBoolWithFlags(HKEY_CURRENT_USER, LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize)", L"SystemUsesLightTheme");
	}

	bool StoreContextMenuDataForItem([[maybe_unused]] HWND hWnd, [[maybe_unused]] ULONG_PTR itemData, [[maybe_unused]] UINT itemID, [[maybe_unused]] ContextMenuRenderingData* cmrd) noexcept
	{
#if ICM_NOSTOREMENUDATA
		return true;
#else
		wchar_t propName[64]; // ImmersiveContextMenuArray_FFFFFFFFFFFFFFFF-4294967295
		if (swprintf_s(propName, L"ImmersiveContextMenuArray_%" PRIXPTR "-%lu", itemData, itemID) != -1)
			return SetPropW(hWnd, propName, reinterpret_cast<HANDLE>(cmrd));
		return false;
#endif
	}

	UINT _GetDpiForMonitorFromPoint(LPPOINT point) noexcept
	{
		POINT pt;
		if (point)
			pt = *point;
		else
			pt = {};

		UINT dpiX, dpiY;
		if (FAILED(GetDpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), MDT_DEFAULT, &dpiX, &dpiY)))
			dpiX = USER_DEFAULT_SCREEN_DPI;
		return dpiX;
	}

	HRESULT _GetRenderingDataForMenuItem(ScaleType type, LPMENUITEMINFOW mii, std::wstring&& itemText, LPPOINT point, CMRDArray* cmrdArray, ContextMenuRenderingData* parentCmrd, ImmersiveContextMenuOptions icmoFlags, ContextMenuRenderingData** cmrd) noexcept
	{
		*cmrd = nullptr;
		auto renderData = new(std::nothrow) ContextMenuRenderingData;
		if (renderData)
		{
			renderData->text = std::move(itemText);
			renderData->hbmpItem = mii->hbmpItem;
			//renderData->hbmpChecked = mii->hbmpChecked;
			//renderData->hbmpUnchecked = mii->hbmpUnchecked;

			if (parentCmrd)
				renderData->dpi = parentCmrd->dpi;
			else if (cmrdArray->empty())
				renderData->dpi = _GetDpiForMonitorFromPoint(point);
			else
				renderData->dpi = cmrdArray->front()->dpi;

			if (WI_IsFlagSet(mii->fType, MFT_SEPARATOR))
				renderData->menuFlags = MFT_SEPARATOR;
			else if (mii->hSubMenu && WI_IsFlagClear(icmoFlags, ICMO_MENUBAR))
				renderData->menuFlags = SUBMENU;
			else if (WI_IsFlagSet(icmoFlags, ICMO_MENUBAR))
				renderData->menuFlags = MENUBAR;
			else
				renderData->menuFlags = 0;
			WI_SetFlagIf(renderData->menuFlags, MFT_RADIOCHECK, WI_IsFlagSet(mii->fType, MFT_RADIOCHECK));

			renderData->cmpt = CMPT_NONE;
			renderData->scaleType = type;
			renderData->useDarkTheme = shouldUseDarkTheme;
			renderData->useSystemPadding = WI_IsFlagSet(icmoFlags, ICMO_USESYSTEMTHEME);

			*cmrd = renderData;

			return S_OK;
		}
		return E_OUTOFMEMORY;
	}

	void _RemoveOwnerDrawFromMenuWorker(HMENU hMenu, HWND hWnd) noexcept
	{
		for (UINT i = 0; ; ++i)
		{
			MENUITEMINFOW mii = {
				.cbSize = sizeof(mii),
				.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_SUBMENU | MIIM_ID
			};
			if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii))
				break;

			mii.fMask = MIIM_FTYPE | MIIM_BITMAP;
			WI_ClearFlag(mii.fType, MFT_OWNERDRAW);

			if (mii.dwItemData)
			{
				auto renderData = GetContextMenuDataForItem(hWnd, mii.dwItemData, mii.wID);
				if (renderData)
				{
					RemoveContextMenuDataForItem(hWnd, mii.dwItemData, mii.wID);

					if (renderData == reinterpret_cast<decltype(renderData)>(mii.dwItemData))
					{
						mii.dwItemData = 0;
						WI_SetFlag(mii.fMask, MIIM_DATA);
					}
					if (renderData->hbmpItem)
					{
						mii.hbmpItem = renderData->hbmpItem;
						renderData->hbmpItem = nullptr;
					}
				}
			}

			SetMenuItemInfoW(hMenu, i, TRUE, &mii);

			if (mii.hSubMenu)
				_RemoveOwnerDrawFromMenuWorker(mii.hSubMenu, hWnd);
		}
	}

	void _RemoveParentArrayFromWindow(HWND hWnd) noexcept
	{
		RemovePropW(hWnd, L"ImmersiveContextMenuArray");
	}

	CMRDArray* _RetrieveParentArrayFromWindow(HWND hWnd) noexcept
	{
		return reinterpret_cast<CMRDArray*>(GetPropW(hWnd, L"ImmersiveContextMenuArray"));
	}

	bool _StoreParentArrayOnWindow(HWND hWnd, CMRDArray* cmrdArray) noexcept
	{
		return SetPropW(hWnd, L"ImmersiveContextMenuArray", reinterpret_cast<HANDLE>(cmrdArray));
	}

	HRESULT _DrawMenuItemText(HTHEME hTheme, POPUPITEMSTATES popupItemState, LPDRAWITEMSTRUCT dis, ContextMenuRenderingData* renderData, LPRECT rcText)
	{
		wil::unique_hdc hCompatibleDC(CreateCompatibleDC(dis->hDC));
		RETURN_LAST_ERROR_IF_NULL(hCompatibleDC);

		const auto width = rcText->right - rcText->left, height = rcText->bottom - rcText->top;

		BITMAPINFO bmi = { .bmiHeader = {
			.biSize = sizeof(BITMAPINFOHEADER),
			.biWidth = width,
			.biHeight = -height,
			.biPlanes = 1,
			.biBitCount = 32,
			.biCompression = BI_RGB
		} };
		void* bits;
		wil::unique_hbitmap hBitmap(CreateDIBSection(hCompatibleDC.get(), &bmi, DIB_RGB_COLORS, &bits, nullptr, 0));
		RETURN_IF_NULL_ALLOC(hBitmap);

		auto select = wil::SelectObject(hCompatibleDC.get(), hBitmap.get());

		const std::wstring_view text = renderData->text;
		const auto pos = text.find_first_of(L'\t');
		auto before = text.substr(0, pos);
		auto after = (pos == std::wstring_view::npos) ? std::wstring_view() : text.substr(pos);

		DWORD textFlags = DT_SINGLELINE | DT_VCENTER;
		WI_SetFlagIf(textFlags, DT_HIDEPREFIX, !renderData->forceAccelerators && WI_IsFlagSet(dis->itemState, ODS_NOACCEL));
		WI_SetFlagIf(textFlags, DT_CENTER, WI_IsFlagSet(renderData->menuFlags, MENUBAR));
		RECT rc = {
			.right = width,
			.bottom = height
		};
		const LONG fontWeight = WI_IsFlagSet(dis->itemState, ODS_DEFAULT) ? FW_BOLD : FW_NORMAL;
		if (!before.empty())
			RETURN_IF_FAILED(DrawThemeTextWithFontWeight(hTheme, hCompatibleDC.get(), MENU_POPUPITEM, popupItemState, before, textFlags | DT_LEFT, &rc, fontWeight));
		if (!after.empty())
			RETURN_IF_FAILED(DrawThemeTextWithFontWeight(hTheme, hCompatibleDC.get(), MENU_POPUPITEM, popupItemState, after, textFlags | DT_RIGHT, &rc, fontWeight));

		RETURN_IF_WIN32_BOOL_FALSE(GdiAlphaBlend(dis->hDC, rcText->left, rcText->top, width, height, hCompatibleDC.get(), 0, 0, width, height, { 0, 0, 255, AC_SRC_ALPHA }));

		return S_OK;
	}

	void _DrawGlyph(HWND hWnd, UINT dpi, HTHEME hTheme, HDC hdc, MENUPARTS part, UINT menuFlags, POPUPITEMSTATES popupItemState, LPRECT rect, ScaleType type)
	{
		wchar_t glyph = 0;
		int fontHeight = -13;
		bool fixFontHeight = false;
		switch (part)
		{
		case MENU_POPUPCHECK:
			glyph = L'\xE0E7'; // Check mark
			if (WI_IsFlagSet(menuFlags, MFT_RADIOCHECK))
			{
				glyph = L'\xE915'; // Radio dot
				fontHeight = -16;
			}
			break;
		case MENU_POPUPSUBMENU:
			fixFontHeight = true;
			glyph = L'\xE76C'; // Right arrow
			// glyph = L'\xE76B'; // Left arrow
			break;
		}

		fontHeight = ScaleByType(type, fontHeight, hWnd, dpi);
		if (fixFontHeight)
		{
			if (!(fontHeight % 2)) // Is even
				--fontHeight;
		}

		const LOGFONTW font = {
			.lfHeight = fontHeight,
			.lfWeight = FW_NORMAL,
			.lfCharSet = DEFAULT_CHARSET,
			.lfQuality = CLEARTYPE_QUALITY,
			.lfFaceName = L"Segoe MDL2 Assets"
		};
		wil::unique_hfont hFont(CreateFontIndirectW(&font));
		if (hFont)
		{
			const auto oldBkMode = SetBkMode(hdc, TRANSPARENT);

			COLORREF color;
			GetThemeColor(hTheme, MENU_POPUPITEM, popupItemState, TMT_TEXTCOLOR, &color);
			const auto oldTextColor = SetTextColor(hdc, color);

			const auto select = wil::SelectObject(hdc, hFont.get());
			DrawTextW(hdc, &glyph, 1, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

			SetTextColor(hdc, oldTextColor);
			SetBkMode(hdc, oldBkMode);
		}
	}

	bool _OnDrawItem(HWND hWnd, LPDRAWITEMSTRUCT dis)
	{
		auto renderData = GetContextMenuDataForItem(hWnd, dis->itemData, dis->itemID);
		if (renderData)
		{
			const wchar_t* className = L"DarkMode_ImmersiveStart::Menu";
			if (!renderData->useDarkTheme)
			{
				if (renderData->useSystemPadding)
					className = L"LightMode_ImmersiveStart::Menu";
				else
					className = L"ImmersiveStart::Menu";
			}
			wil::unique_htheme hTheme(OpenThemeDataForDpiWithFallback(nullptr, className, renderData->scaleType == PPI ? GetDpiForWindow(hWnd) : renderData->dpi));
			if (hTheme)
			{
				if (WI_IsFlagSet(renderData->cmpt, CMPT_TOP_PADDING))
				{
					UINT height = ContextMenuDefinitions::topPaddingPixels;
					if (renderData->useSystemPadding || WI_IsFlagSet(renderData->cmpt, CMPT_TOUCH_INPUT))
						height = ContextMenuDefinitions::topPaddingPixelsWithPaddingOrTouch;
					dis->rcItem.top += ScaleByType(renderData->scaleType, height, hWnd, renderData->dpi);
				}
				if (WI_IsFlagSet(renderData->cmpt, CMPT_BOTTOM_PADDING))
				{
					UINT height = ContextMenuDefinitions::bottomPaddingPixels;
					if (renderData->useSystemPadding || WI_IsFlagSet(renderData->cmpt, CMPT_TOUCH_INPUT))
						height = ContextMenuDefinitions::bottomPaddingPixelsWithPaddingOrTouch;
					dis->rcItem.bottom -= ScaleByType(renderData->scaleType, height, hWnd, renderData->dpi);
				}

				// Mouse Hover
				bool isHot = WI_IsAnyFlagSet(dis->itemState, ODS_SELECTED | ODS_HOTLIGHT) && !renderData->text.empty();
				POPUPITEMSTATES stateId = isHot ? MPI_HOT : MPI_NORMAL;
				if (WI_IsFlagSet(dis->itemState, ODS_DISABLED))
					stateId = isHot ? MPI_DISABLEDHOT : MPI_DISABLED;
				if (SUCCEEDED(DrawThemeBackground(hTheme.get(), dis->hDC, MENU_POPUPITEM, stateId, &dis->rcItem, nullptr)))
				{
					if (renderData->menuFlags != MFT_SEPARATOR)
					{
						if (!renderData->text.empty())
						{
							RECT rect = dis->rcItem;
							if (!WI_IsFlagSet(renderData->menuFlags, MENUBAR))
							{
								rect.left += ScaleByType(renderData->scaleType, ContextMenuDefinitions::totalIconHorizontalPixels, hWnd, renderData->dpi);
								rect.right -= ScaleByType(renderData->scaleType, ContextMenuDefinitions::textRightPaddingPixels, hWnd, renderData->dpi);
							}

							_DrawMenuItemText(hTheme.get(), stateId, dis, renderData, &rect);
						}

						const bool isSubmenu = WI_IsFlagSet(renderData->menuFlags, SUBMENU);
						const bool isChecked = WI_IsFlagSet(dis->itemState, ODS_CHECKED);
						if (isSubmenu || isChecked)
						{
							RECT rect = dis->rcItem;

							const auto height = rect.bottom - rect.top;
							const auto scaledIconSize = ScaleByType(renderData->scaleType, ContextMenuDefinitions::iconSize, hWnd, renderData->dpi);
							const auto iconTop = (height - scaledIconSize) / 2; // Center icon
							rect.top += iconTop;
							rect.bottom = rect.top + scaledIconSize;

							if (isSubmenu)
							{
								rect.right -= ScaleByType(renderData->scaleType, ContextMenuDefinitions::rightPaddingPixels, hWnd, renderData->dpi);
								rect.left = rect.right - scaledIconSize;

								_DrawGlyph(hWnd, renderData->dpi, hTheme.get(), dis->hDC, MENU_POPUPSUBMENU, renderData->menuFlags, stateId, &rect, renderData->scaleType);
							}

							if (isChecked)
							{
								rect.left = dis->rcItem.left + ScaleByType(renderData->scaleType, ContextMenuDefinitions::iconHorizontalPadding, hWnd, renderData->dpi);
								rect.right = rect.left + scaledIconSize;

								_DrawGlyph(hWnd, renderData->dpi, hTheme.get(), dis->hDC, MENU_POPUPCHECK, renderData->menuFlags, stateId, &rect, renderData->scaleType);
							}
						}
					}
					else
					{
						RECT rect = dis->rcItem;
						const auto padding = ScaleByType(renderData->scaleType, ContextMenuDefinitions::separatorHorizontalPaddingPixels, hWnd, renderData->dpi);
						rect.left += padding;
						rect.right -= padding;

						// The separator image is top aligned, here we make it centered.
						const auto height = rect.bottom - rect.top;
						const auto topOffset = (height - ContextMenuDefinitions::separatorBackgroundVerticalPixels) / 2;
						rect.top += topOffset;
						--rect.bottom;
						DrawThemeBackground(hTheme.get(), dis->hDC, MENU_POPUPSEPARATOR, stateId, &rect, nullptr);
					}
					ExcludeClipRect(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom); // Prevent system draw on current rect
					return true;
				}
			}
		}
		return false;
	}

	bool _OnMeasureItem(HWND hWnd, LPMEASUREITEMSTRUCT mis)
	{
		auto renderData = GetContextMenuDataForItem(hWnd, mis->itemData, mis->itemID);
		if (renderData)
		{
			const wchar_t* className = L"DarkMode_ImmersiveStart::Menu";
			if (!renderData->useDarkTheme)
			{
				if (renderData->useSystemPadding)
					className = L"LightMode_ImmersiveStart::Menu";
				else
					className = L"ImmersiveStart::Menu";
			}
			wil::unique_htheme hTheme(OpenThemeDataForDpiWithFallback(nullptr, className, renderData->scaleType == PPI ? GetDpiForWindow(hWnd) : renderData->dpi));
			if (hTheme)
			{
				if (renderData->menuFlags == MFT_SEPARATOR)
				{
					UINT height = ContextMenuDefinitions::separatorVerticalPixels;
					if (renderData->useSystemPadding || WI_IsFlagSet(renderData->cmpt, CMPT_TOUCH_INPUT))
						height = ContextMenuDefinitions::separatorVerticalPixelsWithPadding;
					mis->itemHeight = ScaleByType(renderData->scaleType, height, hWnd, renderData->dpi);
					return true;
				}

				HWND hWndMenu = FindWindowW(L"#32768", nullptr);
				if (!hWndMenu)
					hWndMenu = hWnd; // Menu Bar

				auto hdc = wil::GetDC(hWndMenu);
				if (hdc)
				{
					NONCLIENTMETRICSW ncm = { sizeof(ncm) };
					if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0))
					{
						const LOGFONTW* font = &ncm.lfMenuFont;
						LOGFONTW themeFont;
						if (SUCCEEDED(GetThemeFont(hTheme.get(), nullptr, MENU_POPUPITEM, 0, TMT_FONT, &themeFont)))
						{
							if (ncm.lfMenuFont.lfHeight <= 0)
							{
								if (ncm.lfMenuFont.lfHeight < themeFont.lfHeight)
									themeFont.lfHeight = ncm.lfMenuFont.lfHeight;
							}
							else if (ncm.lfMenuFont.lfHeight > themeFont.lfHeight)
								themeFont.lfHeight = ncm.lfMenuFont.lfHeight;
						}
						else
							font = &themeFont;

						wil::unique_hfont hFont(CreateFontIndirectW(font));
						if (hFont)
						{
							auto select = wil::SelectObject(hdc.get(), hFont.get());

							RECT textRect = {};
							if (SUCCEEDED(GetThemeTextExtent(hTheme.get(), hdc.get(), MENU_POPUPITEM, MPI_NORMAL, renderData->text.c_str(), static_cast<int>(renderData->text.size()), DT_SINGLELINE | DT_VCENTER, nullptr, &textRect)))
							{
								const UINT textWidth = textRect.right - textRect.left;
								UINT textHeight = textRect.bottom - textRect.top;
								if (textHeight == ContextMenuDefinitions::textVerticalPixelsToFix)
									--textHeight;

								mis->itemWidth = textWidth;
								if (WI_IsFlagClear(renderData->menuFlags, MENUBAR))
									mis->itemWidth += ScaleByType(renderData->scaleType, ContextMenuDefinitions::nonTextHorizontalPixels, hWnd, renderData->dpi);

								UINT height = ContextMenuDefinitions::nonTextVerticalPixelsTouchInput;
								if (WI_IsFlagClear(renderData->cmpt, CMPT_TOUCH_INPUT))
								{
									if (renderData->useSystemPadding)
										height = ContextMenuDefinitions::nonTextVerticalPixelsWithPadding;
									else
										height = ContextMenuDefinitions::nonTextVerticalPixels;
								}
								mis->itemHeight = textHeight + ScaleByType(renderData->scaleType, height, hWnd, renderData->dpi);
							}
						}
					}
				}
				if (WI_IsFlagSet(renderData->cmpt, CMPT_TOP_PADDING))
				{
					UINT height = ContextMenuDefinitions::topPaddingPixels;
					if (renderData->useSystemPadding || WI_IsFlagSet(renderData->cmpt, CMPT_TOUCH_INPUT))
						height = ContextMenuDefinitions::topPaddingPixelsWithPaddingOrTouch;
					mis->itemHeight += ScaleByType(renderData->scaleType, height, hWnd, renderData->dpi);
				}
				if (WI_IsFlagSet(renderData->cmpt, CMPT_BOTTOM_PADDING))
				{
					UINT height = ContextMenuDefinitions::bottomPaddingPixels;
					if (renderData->useSystemPadding || WI_IsFlagSet(renderData->cmpt, CMPT_TOUCH_INPUT))
						height = ContextMenuDefinitions::bottomPaddingPixelsWithPaddingOrTouch;
					mis->itemHeight += ScaleByType(renderData->scaleType, height, hWnd, renderData->dpi);
				}
				return true;
			}
		}
		return false;
	}

	bool ContextMenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
	{
		bool handled = false;
		switch (message)
		{
		case WM_DRAWITEM:
			handled = _OnDrawItem(hWnd, reinterpret_cast<LPDRAWITEMSTRUCT>(lParam));
			break;
		case WM_MEASUREITEM:
			handled = _OnMeasureItem(hWnd, reinterpret_cast<LPMEASUREITEMSTRUCT>(lParam));
			break;
		case WM_INITMENUPOPUP:
			HandleMergedMenus(reinterpret_cast<HMENU>(wParam), hWnd);
			break;
		}
		return handled;
	}
}
