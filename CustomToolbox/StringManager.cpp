#include "pch.h"
#include "StringManager.h"

std::string StringManager::getMultiByte(std::wstring str) {
	int needed_size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, NULL, NULL);
	LPSTR result = (LPSTR)malloc(needed_size * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, result, needed_size, NULL, NULL);
	std::string s(result);
	free(result);
	return s;
}

std::wstring StringManager::getWideStr(std::string str) {
	int needed_size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	LPWSTR result = (LPWSTR)malloc(needed_size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result, needed_size);
	std::wstring s(result);
	free(result);
	return s;
}