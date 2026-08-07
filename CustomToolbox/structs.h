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
	std::wstring iconName;
	bool valid;

	bool operator<(const Effect& another) const {
		return index < another.index;
	}
};