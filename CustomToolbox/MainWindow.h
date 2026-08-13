#pragma once

#include <SDK/plugin2.h>
#include <SDK/logger2.h>
#include <SDK/config2.h>

#include "structs.h"
#include "Effects.h"

#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#include "Settings.h"

class MainWindow
{
private:
	static HWND hwnd;

	static SCROLLINFO si;		//このウィンドウのスクロール情報
	static RECT rect;			//クライアント領域はここに収納
	static PAINTSTRUCT ps;		//BeginPaintするときはこれを使う
	static HDC hdc;				//このウィンドウのデバイスコンテキストはここに収納

	static std::wstring pluginDir;

	static EDIT_HANDLE* edit_handle;
	static LOG_HANDLE* logger;
	static CONFIG_HANDLE* config;

	static std::vector<RawEffect> rawEffects;
	//static Effects effects;
	static std::vector<Effect> validEffects;

	static LPCSTR alias;

	//static int iconWidth;
	//static int iconHeight;
	static Settings settings;

	static int index;
	static bool dragged;

public:

	static void setLogger(LOG_HANDLE* handle);
	static void setConfig(CONFIG_HANDLE* handle);

	static void initDLL(HINSTANCE hInstDLL);

	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static void registerPlugin(HOST_APP_TABLE* host);

	static int getIndexFromPosition(int x, int y);
	static COLORREF getColorRef(int color);
};

