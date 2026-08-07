#include "pch.h"
#include "Effects.h"

void Effects::init(std::vector<RawEffect> rawEffects) {
	for (int i = 0; i < rawEffects.size(); i++) {
		addEffect(rawEffects[i]);
	}
	sort();
}

int Effects::validEffectsNum() {
	int num = 0;
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].valid) {
			num++;
		}
	}
	return num;
}

int Effects::invalidEffectsNum() {
	int num = 0;
	for (int i = 0; i < effects.size(); i++) {
		if (!effects[i].valid) {
			num++;
		}
	}
	return num;
}

int Effects::allEffectsNum() {
	return effects.size();
}

bool Effects::setIndex(const RawEffect& effect, int index) {
	int currentIndex = lookUpEffect(effect);
	if (currentIndex == -1) {
		return false;
	}
	else if (currentIndex < index) {
		Effect e = effects[currentIndex];
		for (int i = currentIndex; i < index; i++) {
			effects[i + 1].index--;
			effects[i] = effects[i + 1];
		}
		e.index = index;
		effects[index] = e;
	}
	else if (currentIndex > index) {
		Effect e = effects[currentIndex];
		for (int i = currentIndex; i > index; i--) {
			effects[i - 1].index--;
			effects[i] = effects[i - 1];
		}
		e.index = index;
		effects[index] = e;
	}
	return true;
}

int Effects::lookUpEffect(const RawEffect& effect) {
	for (int i = 0; i < effects.size(); i++) {
		Effect e = effects[i];
		if ((e.name == effect.name) && (e.flag == effect.flag) && (e.type == effect.type)) {
			return i;
		}
	}
	return -1;
	
}

int Effects::addEffect(const RawEffect& effect, bool valid) {
	Effect e = {};
	e.name = effect.name;
	e.type = effect.type;
	e.flag = effect.flag;
	e.valid = valid;
	e.index = effects.size();
	effects.push_back(e);
	return e.index;
}

std::vector<Effect> Effects::getValidEffects() {
	std::vector<Effect> vec;
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].valid) {
			vec.push_back(effects[i]);
		}
	}
	return vec;
}

Effect& Effects::operator[](int index) {
	return effects[index];
}

void Effects::sort() {
	std::sort(effects.begin(), effects.end());
}