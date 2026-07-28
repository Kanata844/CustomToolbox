#pragma once

#include <string>

struct RawEffect {
	std::wstring name;
	int type;
	int flag;
};

struct Effect {
	int index;
	std::wstring name;
	int type;
	int flag;
	std::string iconName;
};