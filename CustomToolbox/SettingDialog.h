#pragma once

class SettingDialog
{
private:
	static LRESULT CALLBACK DialogWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
	static void display(HINSTANCE hInst, HWND hWnd);
};

