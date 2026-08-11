#include "pch.h"
#include "SettingDialog.h"

LRESULT CALLBACK SettingDialog::DialogWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static HWND hList = NULL;
	static HWND hRadioAll = NULL;
	static HWND hRadioValid = NULL;
	static HWND hRadioInvalid = NULL;
	switch (msg) {
	case WM_INITDIALOG: {
		hList = GetDlgItem(hWnd, IDC_LIST1);
		hRadioAll = GetDlgItem(hWnd, IDC_RADIO_ALL);
		hRadioValid = GetDlgItem(hWnd, IDC_RADIO_VALID);
		hRadioInvalid = GetDlgItem(hWnd, IDC_RADIO_INVALID);

		LONG lStyle = GetWindowLong(hList, GWL_STYLE);
		lStyle |= LVS_REPORT;
		SetWindowLong(hList, GWL_STYLE, lStyle);

		DWORD dwStyle = ListView_GetExtendedListViewStyle(hList);
		dwStyle |= LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
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

		switch (displayMode) {
		case DisplayMode::ALL_EFFECTS:
			Button_SetCheck(hRadioAll, TRUE);
			break;
		case DisplayMode::VALID_EFFECTS:
			Button_SetCheck(hRadioValid, TRUE);
			break;
		case DisplayMode::INVALID_EFFECTS:
			Button_SetCheck(hRadioInvalid, TRUE);
			break;
		}

		setEffects(hList);

		return TRUE;
	}
	case WM_NOTIFY: {
		if (wparam == IDC_LIST1) {
			switch (((LV_DISPINFO*)lparam)->hdr.code) {
			case LVN_ITEMCHANGED: {
				if (!initializing) {	//一番最初の初期化中は何も行わない
					int iItem = ((NM_LISTVIEW*)lparam)->iItem;
					if (iItem < 0) {
						break;
					}
					bool itemChecked = ListView_GetCheckState(hList, iItem);
					switch (displayMode) {
					case DisplayMode::ALL_EFFECTS: {
						copy[iItem].valid = itemChecked;
						break;
					}
					case DisplayMode::VALID_EFFECTS: {
						auto validEffects = copy.getValidEffects();
						if (validEffects[iItem].valid != itemChecked) {
							copy[validEffects[iItem].index].valid = false;
							ListView_DeleteItem(hList, iItem);
						}
						break;
					}
					case DisplayMode::INVALID_EFFECTS: {
						auto invalidEffects = copy.getInvalidEffects();
						if (invalidEffects[iItem].valid != itemChecked) {
							copy[invalidEffects[iItem].index].valid = true;
							ListView_DeleteItem(hList, iItem);
						}
						break;
					}
					}
				}
				break;
			}
			}
		}
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDC_BUTTON_UP: {
			int iItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
			if (iItem < 0) {
				break;
			}
			else {
				switch (displayMode) {
				case DisplayMode::ALL_EFFECTS: {
					if (copy.changeIndex(iItem, iItem - 1)) {
						updateEffect(hList, { iItem, iItem - 1 });
						ListView_SetItemState(hList, iItem, NULL, LVIS_SELECTED);
						ListView_SetItemState(hList, iItem - 1, LVIS_SELECTED, LVIS_SELECTED);
						ListView_Scroll(hList, 0, -16);
					}
					break;
				}
				case DisplayMode::VALID_EFFECTS: {
					if (iItem == 0) {
						break;
					}
					auto validEffects = copy.getValidEffects();
					if (copy.changeIndex(validEffects[iItem].index, validEffects[iItem - 1].index)) {
						updateEffect(hList, { iItem, iItem - 1 });
						ListView_SetItemState(hList, iItem, NULL, LVIS_SELECTED);
						ListView_SetItemState(hList, iItem - 1, LVIS_SELECTED, LVIS_SELECTED);
						ListView_Scroll(hList, 0, -16);
					}
					break;
				}
				case DisplayMode::INVALID_EFFECTS: {
					if (iItem == 0) {
						break;
					}
					auto invalidEffects = copy.getInvalidEffects();
					if (copy.changeIndex(invalidEffects[iItem].index, invalidEffects[iItem - 1].index)) {
						updateEffect(hList, { iItem, iItem - 1 });
						ListView_SetItemState(hList, iItem, NULL, LVIS_SELECTED);
						ListView_SetItemState(hList, iItem - 1, LVIS_SELECTED, LVIS_SELECTED);
						ListView_Scroll(hList, 0, -16);
					}
					break;
				}
				}
				
			}
			break;
		}
		case IDC_BUTTON_DOWN: {
			int iItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
			if (iItem < 0) {
				break;
			}
			else {
				switch (displayMode) {
				case DisplayMode::ALL_EFFECTS: {
					if (copy.changeIndex(iItem, iItem + 1)) {
						updateEffect(hList, { iItem, iItem + 1 });;
						ListView_SetItemState(hList, iItem, NULL, LVIS_SELECTED);
						ListView_SetItemState(hList, iItem + 1, LVIS_SELECTED, LVIS_SELECTED);
						ListView_Scroll(hList, 0, 16);
					}
					break;
				}
				case DisplayMode::VALID_EFFECTS: {
					auto validEffects = copy.getValidEffects();
					if (iItem >= validEffects.size() - 1) {
						break;
					}
					if (copy.changeIndex(validEffects[iItem].index, validEffects[iItem + 1].index)) {
						updateEffect(hList, { iItem, iItem + 1 });
						ListView_SetItemState(hList, iItem, NULL, LVIS_SELECTED);
						ListView_SetItemState(hList, iItem + 1, LVIS_SELECTED, LVIS_SELECTED);
						ListView_Scroll(hList, 0, 16);
					}
					break;
				}
				case DisplayMode::INVALID_EFFECTS: {
					auto invalidEffects = copy.getInvalidEffects();
					if (iItem >= invalidEffects.size() - 1) {
						break;
					}
					if (copy.changeIndex(invalidEffects[iItem].index, invalidEffects[iItem + 1].index)) {
						updateEffect(hList, { iItem, iItem + 1 });
						ListView_SetItemState(hList, iItem, NULL, LVIS_SELECTED);
						ListView_SetItemState(hList, iItem + 1, LVIS_SELECTED, LVIS_SELECTED);
						ListView_Scroll(hList, 0, 16);
					}
					break;
				}
				}
				
			}
			break;
		}
		case IDC_RADIO_ALL: {
			displayMode = DisplayMode::ALL_EFFECTS;
			setEffects(hList);
			break;
		}
		case IDC_RADIO_VALID: {
			displayMode = DisplayMode::VALID_EFFECTS;
			setEffects(hList);
			break;
		}
		case IDC_RADIO_INVALID: {
			displayMode = DisplayMode::INVALID_EFFECTS;
			setEffects(hList);
			break;
		}
		case IDOK:
			*ep = copy;
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

void SettingDialog::setEffects(HWND hList) {
	if (!ep) return;

	initializing = true;

	ListView_DeleteAllItems(hList);

	LV_ITEM item;
	std::wstring name;
	item.mask = LVIF_TEXT;

	std::vector<Effect> effects;

	switch(displayMode){
	case DisplayMode::ALL_EFFECTS:
		effects = copy.getAllEffects();
		break;
	case DisplayMode::VALID_EFFECTS:
		effects = copy.getValidEffects();
		break;
	case DisplayMode::INVALID_EFFECTS:
		effects = copy.getInvalidEffects();
		break;
	}

	for (int i = 0; i < effects.size(); i++) {
		name = effects[i].name;
		item.pszText = name.data();
		item.iItem = i;
		item.iSubItem = 0;
		ListView_InsertItem(hList, &item);

		name = std::to_wstring(effects[i].type);
		item.pszText = name.data();
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);

		name = std::to_wstring(effects[i].flag);
		item.pszText = name.data();
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);
		ListView_SetCheckState(hList, i, effects[i].valid);
	}

	initializing = false;
	
}

void SettingDialog::updateEffect(HWND hList, std::vector<int> indices) {
	LV_ITEM item;
	std::wstring name;
	item.mask = LVIF_TEXT;

	std::vector<Effect> effects;

	switch (displayMode) {
	case DisplayMode::ALL_EFFECTS: {
		effects = copy.getAllEffects();
		break;
	}
	case DisplayMode::VALID_EFFECTS: {
		effects = copy.getValidEffects();
		break;
	}
	case DisplayMode::INVALID_EFFECTS: {
		effects = copy.getInvalidEffects();
		break;
	}
	}

	for (int i: indices) {
		name = effects[i].name;
		item.pszText = name.data();
		item.iItem = i;
		item.iSubItem = 0;
		ListView_SetItem(hList, &item);

		name = std::to_wstring(effects[i].type);
		item.pszText = name.data();
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);

		name = std::to_wstring(effects[i].flag);
		item.pszText = name.data();
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);
		ListView_SetCheckState(hList, i, effects[i].valid);
	}
	
}

void SettingDialog::display(HINSTANCE hInst, HWND hWnd, Effects* pEffects) {
	ep = pEffects;
	copy = *ep;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogWndProc);
}

Effects* SettingDialog::ep = nullptr;
Effects SettingDialog::copy;
bool SettingDialog::initializing = false;
//bool SettingDialog::validOnly = false;
DisplayMode SettingDialog::displayMode = DisplayMode::ALL_EFFECTS;