#pragma once

#include "Settings.h"
#include "StringManager.h"

#include <string>
#include <fstream>
#include <vector>

#include <nlohmann/json.hpp>

class SettingsManager
{
public:
	//指定したファイルに設定を保存する。成功したらtrue、ファイルをオープンできなければfalseを返す。
	static bool saveToFile(Settings* ps, std::wstring fileName);
	//指定したファイルから設定を読み込む。読み込めたらtrue、読み込めなかったらfalseを返す。
	static bool loadFromFile(Settings* ps, std::wstring fileName);
};

