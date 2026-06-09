#pragma once

#include "plugin2.h"
#include "logger2.h"
#include "config2.h"

#include <vector>
#include <string>

class MainWindow
{
private:
	static HWND hwnd;

	static EDIT_HANDLE* edit_handle;
	static LOG_HANDLE* logger;
	static CONFIG_HANDLE* config;

	static std::vector<std::wstring> effects;
public:
	static LPCSTR alias;

	static void setLogger(LOG_HANDLE* handle);
	static void setConfig(CONFIG_HANDLE* handle);

	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static void registerPlugin(HOST_APP_TABLE* host);
};

