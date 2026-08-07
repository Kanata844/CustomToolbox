#include "pch.h"
#include "SettingDialog.h"

#include "resource.h"

#include <string>

#include <Windows.h>
#include <CommCtrl.h>

LRESULT CALLBACK SettingDialog::DialogWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static HWND hList = NULL;
	static LV_ITEM item;
	switch (msg) {
	case WM_INITDIALOG: {
		hList = GetDlgItem(hWnd, IDC_LIST1);

		LONG lStyle = GetWindowLong(hList, GWL_STYLE);
		lStyle |= LVS_REPORT;
		SetWindowLong(hList, GWL_STYLE, lStyle);

		DWORD dwStyle = ListView_GetExtendedListViewStyle(hList);
		dwStyle |= LVS_EX_CHECKBOXES;
		ListView_SetExtendedListViewStyle(hList, dwStyle);

		LV_COLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.cx = 200;
		std::wstring name(L"エフェクト名");
		lvcol.pszText = name.data();
		lvcol.iSubItem = 0;
		ListView_InsertColumn(hList, 0, &lvcol);

		lvcol.cx = 50;
		name = L"タイプ";
		lvcol.pszText = name.data();
		lvcol.iSubItem = 1;
		ListView_InsertColumn(hList, 1, &lvcol);

		lvcol.cx = 50;
		name = L"フラグ";
		lvcol.pszText = name.data();
		lvcol.iSubItem = 2;
		ListView_InsertColumn(hList, 2, &lvcol);

		item.mask = LVIF_TEXT;
		name = L"ああああ";
		item.pszText = name.data();
		item.iItem = 0;
		item.iSubItem = 0;
		ListView_InsertItem(hList, &item);

		name = L"1";
		item.pszText = name.data();
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);

		name = L"1";
		item.pszText = name.data();
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);

		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			EndDialog(hWnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

void SettingDialog::display(HINSTANCE hInst, HWND hWnd) {
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogWndProc);
}