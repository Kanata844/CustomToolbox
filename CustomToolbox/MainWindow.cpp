#include "pch.h"
#include "MainWindow.h"

#include "SettingDialog.h"

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <ole2.h>

#pragma comment(lib, "comctl32.lib")

#define IDC_BUTTON 1001
#define SampleWindowName L"CustomToolbox"

void MainWindow::setLogger(LOG_HANDLE* handle) {
	logger = handle;
}

void MainWindow::setConfig(CONFIG_HANDLE* handle) {
	config = handle;
}

LRESULT CALLBACK MainWindow::wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	static int index = -1;
	static bool dragged = false;

	switch (message) {
	case WM_CREATE:
		si.nPos = 0;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		InitCommonControls();
		break;
	case WM_DESTROY:

		break;
	case WM_LBUTTONDOWN: {
		SetCapture(hwnd);
		dragged = false;
		int x = GET_X_LPARAM(lparam);
		int y = GET_Y_LPARAM(lparam);
		index = getIndexFromPosition(x, y);
		break;
	}
	case WM_LBUTTONUP: {
		if (!dragged) {
			if (index < 0 || index >= validEffects.size()) break;
			else {
				edit_handle->call_edit_section([](EDIT_SECTION* edit) {
					if (edit->create_object(validEffects[index].name.c_str(), edit->info->layer, edit->info->frame, 30)) {
						logger->log(logger, L"successfully created an object");
					}
					else {
						if (edit->create_object(validEffects[index].name.c_str(), edit->info->layer, ++edit->info->frame, 30)) {
							logger->log(logger, L"successfully created an object");
						}
						else {
							logger->log(logger, L"failed in creating an object");
							--edit->info->frame;
						}
					}
					});
			}
		}
		else {
			dragged = false;
			edit_handle->call_edit_section([](EDIT_SECTION* edit) {
				POINT p;
				GetCursorPos(&p);
				logger->log(logger, (std::to_wstring(p.x) + L" " + std::to_wstring(p.y)).c_str());
				int layer, frame;
				logger->log(logger, std::to_wstring(edit->get_mouse_layer_frame(&layer, &frame)).c_str());
				if (edit->pos_to_layer_frame(p.x, p.y, &layer, &frame)) {
					auto nextObject = edit->find_object(layer, frame);
					if (nextObject == nullptr) {
						edit->create_object(validEffects[index].name.c_str(), layer, frame, 60);
					}
					else {
						auto nextStart = edit->get_object_layer_frame(nextObject).start;
						if (nextStart > frame + 60) {
							edit->create_object(validEffects[index].name.c_str(), layer, frame, 60);
						}
						else if (nextStart > frame) {
							edit->create_object(validEffects[index].name.c_str(), layer, frame, nextStart - frame);
						}
						else {
							if (validEffects[index].flag == edit_handle->EFFECT_TYPE_FILTER) {
								edit->create_effect(nextObject, validEffects[index].name.c_str());
							}
						}
					}
				}
				});
		}
		ReleaseCapture();
		break;
	}
	case WM_MOUSEMOVE: {
		if (!dragged) {
			int x = GET_X_LPARAM(lparam);
			int y = GET_Y_LPARAM(lparam);
			if (getIndexFromPosition(x, y) != index && index >= 0 && index < validEffects.size()) {
				dragged = true;
			}
		}
		break;
	}
	case WM_PAINT: {
		//クライアント領域を取得し、行数、列数を計算する
		GetClientRect(hwnd, &rect);
		int colNum = rect.right / iconWidth;
		if (colNum <= 0) colNum = 1;
		int rowNum = (validEffects.size() - 1) / colNum + 1;
		//SCROLLINFO構造体の情報を変更する
		si.nMax = rowNum - 1;
		si.nPage = min(rect.bottom / iconHeight, si.nMax + 1);
		//SCROLLINFO構造体の変更を適用する
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		//もしスクロールバーがない状態からある状態に変化した場合、再計算しないとおかしくなるので再計算する。
		//絶対もっといいやり方があると思う。
		GetClientRect(hwnd, &rect);
		colNum = rect.right / iconWidth;
		if (colNum <= 0) colNum = 1;
		rowNum = (validEffects.size() - 1) / colNum + 1;
		//SCROLLINFO構造体の情報を変更する
		si.nMax = rowNum - 1;
		si.nPage = min(rect.bottom / iconHeight, si.nMax + 1);
		//SCROLLINFO構造体の変更を適用する
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		//ちらつきを抑えるために、ダブルバッファリングのようなことを行う。まずはバッファを作成する。
		hdc = GetDC(hwnd);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, iconWidth * colNum, iconHeight * rowNum);
		HDC hBuffer = CreateCompatibleDC(hdc);
		ReleaseDC(hwnd, hdc);
		//バッファに描画を開始する
		SelectObject(hBuffer, hBitmap);
		COLORREF bgColor = getColorRef(config->get_color_code(config, "Background"));
		HBRUSH hbrBackgound = CreateSolidBrush(bgColor);
		RECT r = { 0, 0, iconWidth * colNum, iconHeight * rowNum };
		FillRect(hBuffer, &r, hbrBackgound);
		SetBkColor(hBuffer, bgColor);
		HFONT hFont = CreateFont(min(iconWidth, iconHeight), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);
		SelectObject(hBuffer, hFont);
		for (int i = 0; i < validEffects.size(); i++) {
			int col = i % colNum;
			int row = i / colNum;
			SetTextColor(hBuffer, RGB(255 *(validEffects[i].type % 2), 255 * ((validEffects[i].type / 2) % 2), 255 * ((validEffects[i].type / 4) % 2)));
			TextOut(hBuffer, iconWidth * col, iconHeight * row, validEffects[i].name.c_str(), 1);
			DeleteObject(SelectObject(hBuffer, GetStockObject(WHITE_BRUSH)));
		}
		DeleteObject(hFont);
		//Ellipse(hBuffer, 0, 0, 100, 100 - si.nPos * iconHeight);
		//画面に表示する
		BeginPaint(hwnd, &ps);
		FillRect(hdc, &rect, hbrBackgound);	//背景クリアはここでしてやる
		BitBlt(hdc, 0, -iconHeight * si.nPos, iconWidth * colNum, iconHeight * rowNum, hBuffer, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		//CreateしたものはDeleteする
		DeleteObject(hbrBackgound);
		DeleteObject(hBitmap);
		DeleteDC(hBuffer);
		return 0;
	}
	case WM_VSCROLL: {
		switch (LOWORD(wparam)) {
		case SB_TOP: {
			si.nPos = si.nMin;
			break;
		}
		case SB_BOTTOM: {
			si.nPos = si.nMax;
			break;
		}
		case SB_LINEUP: {
			if (si.nPos > 0) si.nPos--;
			break;
		}
		case SB_LINEDOWN: {
			if (si.nPos < si.nMax - si.nPage + 1) si.nPos++;
			break;
		}
		case SB_PAGEUP: {
			si.nPos -= si.nPage;
			if (si.nPos < si.nMin) si.nPos = si.nMin;
			break;
		}
		case SB_PAGEDOWN: {
			si.nPos += si.nPage;
			if (si.nPos > si.nMax - si.nPage + 1) si.nPos = si.nMax - si.nPage + 1;
			break;
		}
		case SB_THUMBPOSITION: {
			si.nPos = HIWORD(wparam);
			break;
		}
		case SB_THUMBTRACK: {
			si.nPos = HIWORD(wparam);
			break;
		}
		}
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	}	
	case WM_MOUSEWHEEL:
		si.nPos -= (short int)HIWORD(wparam) / 120;
		if (si.nPos < si.nMin) si.nPos = si.nMin;
		else if (si.nPos > si.nMax - si.nPage + 1) si.nPos = si.nMax - si.nPage + 1;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_ERASEBKGND:		//背景が勝手に消されるとめっちゃちらつきます
		return 1;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

void MainWindow::registerPlugin(HOST_APP_TABLE* host) {
	// 自身のウィンドウを作成
	WNDCLASSEXW wcex = {};
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpszClassName = SampleWindowName;
	wcex.lpfnWndProc = wnd_proc;
	wcex.hInstance = GetModuleHandle(0);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClassEx(&wcex)) {
		return;
	}
	hwnd = CreateWindowEx(
		0,
		SampleWindowName,
		SampleWindowName,
		WS_POPUP | WS_VSCROLL | WS_EX_COMPOSITED, // 親ウィンドウの指定無しでWS_CHILDが作れないので一旦WS_POPUPで作成しています
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		GetModuleHandle(0),
		nullptr);
	if (!hwnd) {
		return;
	}
	// ボタンの作成
	/*
	CreateWindowEx(
		0,
		WC_BUTTON,
		config->translate(config, L"オブジェクト作成"),
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		10, 10, 200, config->get_layout_size(config, "SettingItemHeight"),
		hwnd,
		(HMENU)IDC_BUTTON,
		GetModuleHandle(0),
		nullptr);
		*/

	// ウィンドウを登録
	host->register_window_client(SampleWindowName, hwnd);

	// 編集ハンドルを作成
	edit_handle = host->create_edit_handle();

	//設定ダイアログを登録
	host->register_config_menu(L"CustomToolbox", [](HWND, HINSTANCE dll_hinst) {
		SettingDialog::display(dll_hinst, hwnd, &effects);
		validEffects = effects.getValidEffects();
		InvalidateRect(hwnd, NULL, TRUE);
		});

	//エフェクト一覧をこの場で取得しようとしてもうまくいかなかったのでこうしてます
	host->register_project_load_handler([](PROJECT_FILE* pf) {
		rawEffects.clear();
		edit_handle->enum_effect_name(nullptr, [](void*, LPCWSTR name, int type, int flag) {
			logger->log(logger, name);
			rawEffects.push_back({name, type, flag});
			});
		effects.init(rawEffects);
		validEffects = effects.getValidEffects();
		InvalidateRect(hwnd, NULL, TRUE);
		});
}

int MainWindow::getIndexFromPosition(int x, int y) {
	if (x < 0 || y < 0 || x > (rect.right / iconWidth) * iconWidth) return -1;
	return x / iconWidth + (y / iconHeight + si.nPos) * (rect.right / iconWidth);
}

COLORREF MainWindow::getColorRef(int color) {
	int r = color >> 16;
	int g = (color >> 8) & 0xff;
	int b = color & 0xff;
	return RGB(r, g, b);
}

HWND MainWindow::hwnd = NULL;

SCROLLINFO MainWindow::si;
RECT MainWindow::rect;
PAINTSTRUCT MainWindow::ps;
HDC MainWindow::hdc = NULL;

EDIT_HANDLE* MainWindow::edit_handle = nullptr;
LOG_HANDLE* MainWindow::logger = nullptr;
CONFIG_HANDLE* MainWindow::config = nullptr;

std::vector<RawEffect> MainWindow::rawEffects;
Effects MainWindow::effects;
std::vector<Effect> MainWindow::validEffects;

int MainWindow::iconHeight = 20;
int MainWindow::iconWidth = 20;

int MainWindow::index = -1;
bool MainWindow::dragged = false;

LPCSTR MainWindow::alias = R"(
[Object]
[Object.0]
effect.name=テキスト
サイズ=150.00
文字色=3bcb48
テキスト=さんぷる
[Object.1]
effect.name=標準描画
[Object.2]
effect.name=縁取り
サイズ=10
縁色=ffffff
)";