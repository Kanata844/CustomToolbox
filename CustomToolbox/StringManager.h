#pragma once
#include <string>

class StringManager
{
public:
	static std::wstring getWideStr(std::string str);
	static std::string getMultiByte(std::wstring str);
};

