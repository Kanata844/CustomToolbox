#include "pch.h"
#include "SettingsManager.h"

using json = nlohmann::json;

bool SettingsManager::saveToFile(Settings* sp, std::wstring fileName) {
	json j_settings;
	j_settings["iconWidth"] = sp->iconWidth;
	j_settings["iconHeight"] = sp->iconHeight;
	json j_effects;
	for (int i = 0; i < sp->effects.allEffectsNum(); i++) {
		json j_effect;
		j_effect["index"] = sp->effects[i].index;
		j_effect["name"] = StringManager::getMultiByte(sp->effects[i].name);
		j_effect["type"] = sp->effects[i].type;
		j_effect["flag"] = sp->effects[i].flag;
		j_effect["iconName"] = StringManager::getMultiByte(sp->effects[i].iconName);
		j_effect["valid"] = sp->effects[i].valid;

		j_effects.push_back(j_effect);
	}
	j_settings["effects"] = j_effects;

	std::ofstream ofs(fileName);
	if (!ofs.is_open()) { 
		return false; 
	}
	ofs << std::setw(4) << j_settings << std::endl;

	return true;
}

bool SettingsManager::loadFromFile(Settings* sp, std::wstring fileName) {
	std::ifstream ifs(fileName);
	if (!ifs.is_open()) return false;

	json j_settings;
	ifs >> j_settings;

	sp->iconHeight = j_settings["iconHeight"];
	sp->iconWidth = j_settings["iconWidth"];
	auto j_effects = j_settings["effects"];
	std::vector<Effect> effects;
	for (int i = 0; i < j_effects.size(); i++) {
		Effect e = {};
		e.flag = j_effects[i]["flag"];
		e.type = j_effects[i]["type"];
		e.index = j_effects[i]["index"];
		e.valid = j_effects[i]["valid"];
		e.name = StringManager::getWideStr(j_effects[i]["name"]);
		e.iconName = StringManager::getWideStr(j_effects[i]["iconName"]);
		effects.push_back(e);
	}
	sp->effects.init(effects);

	return true;
}