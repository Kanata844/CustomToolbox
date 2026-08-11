#pragma once

#include "Effects.h"

#include "resource.h"

#include <string>

#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

enum class DisplayMode {
	ALL_EFFECTS,
	VALID_EFFECTS,
	INVALID_EFFECTS
};

class SettingDialog
{
private:
	static LRESULT CALLBACK DialogWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static void setEffects(HWND hList);
	static void updateEffect(HWND hList, std::vector<int> indices);

	static Effects* ep;
	static Effects copy;

	//static bool validOnly;
	static DisplayMode displayMode;

	static bool initializing;
public:
	static void display(HINSTANCE hInst, HWND hWnd, Effects* pEffects);
};

