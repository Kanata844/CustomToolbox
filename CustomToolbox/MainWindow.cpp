#include "pch.h"
#include "MainWindow.h"

#include <windows.h>
#include <commctrl.h>

#define IDC_BUTTON 1001
#define SampleWindowName L"CustomToolbox"

#define ICON_WIDTH 20
#define ICON_HEIGHT 20

void MainWindow::setLogger(LOG_HANDLE* handle) {
	logger = handle;
}

void MainWindow::setConfig(CONFIG_HANDLE* handle) {
	config = handle;
}

LRESULT CALLBACK MainWindow::wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	static SCROLLINFO si;

	switch (message) {
	case WM_CREATE:
		si.nPos = 0;
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDC_BUTTON:
			for (std::wstring effect : effects) {
				logger->log(logger, effect.c_str());
			}
			// 編集セクションで処理を呼び出す
			edit_handle->call_edit_section_param(&message, [](void* message, EDIT_SECTION* edit) {
				// エイリアスデータからオブジェクトを作成
				if (edit->create_object_from_alias(alias, edit->info->layer, edit->info->frame, 10)) {
					logger->log(logger, L"create alias object");
				}
				else {
					logger->warn(logger, L"create alias failed");
				}
				});
			SetFocus(NULL); // ボタンのフォーカスを外す
			return 0;
		}
		break;
	case WM_PAINT: {
		//クライアント領域を取得し、行数、列数を計算する
		RECT rect;
		GetClientRect(hwnd, &rect);
		int colNum = rect.right / ICON_WIDTH;
		int rowNum = (effects.size() - 1) / colNum + 1;
		//SCROLLINFO構造体の情報を変更する
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = rowNum - 1;
		si.nPage = rect.bottom / ICON_HEIGHT;
		//SCROLLINFO構造体の変更を適用する
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		//描画を行う
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		for (int i = 0; i < effects.size(); i++) {
			int col = i % colNum;
			int row = i / colNum;
			SelectObject(hdc, CreateSolidBrush(0xff << (8 * (i % 3))));
			Rectangle(hdc, ICON_WIDTH * col, ICON_HEIGHT * (row - si.nPos), ICON_WIDTH * (col + 1), ICON_HEIGHT * (row - si.nPos + 1));
			DeleteObject(SelectObject(hdc, GetStockObject(WHITE_BRUSH)));
		}
		Ellipse(hdc, 0, -si.nPos * ICON_HEIGHT, 100, 100 - si.nPos * ICON_HEIGHT);
		EndPaint(hwnd, &ps);
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
		WS_POPUP | WS_VSCROLL, // 親ウィンドウの指定無しでWS_CHILDが作れないので一旦WS_POPUPで作成しています
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

	//エフェクト一覧をこの場で取得しようとしてもうまくいかなかったのでこうしてます
	host->register_project_load_handler([](PROJECT_FILE* pf) {
		effects.clear();
		edit_handle->enum_effect_name(&effects, [](void* effects, LPCWSTR name, int type, int flag) {
			((std::vector<std::wstring>*)effects)->push_back(name);
			InvalidateRect(hwnd, NULL, TRUE);
			});
		});
}

HWND MainWindow::hwnd = NULL;

EDIT_HANDLE* MainWindow::edit_handle = nullptr;
LOG_HANDLE* MainWindow::logger = nullptr;
CONFIG_HANDLE* MainWindow::config = nullptr;

std::vector<std::wstring> MainWindow::effects;

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