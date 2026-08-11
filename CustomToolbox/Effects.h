#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "structs.h"

class Effects
{
public:
	//RawEffectのベクターをもとに初期化する。
	void init(std::vector<RawEffect> rawEffects);
	//有効なエフェクトの数を返す。
	int validEffectsNum();
	//無効なエフェクトの数を返す。
	int invalidEffectsNum();
	//すべてのエフェクトの数を返す。
	int allEffectsNum();

	//第一引数で指定したインデックスのエフェクトを、第二引数で指定したインデックスまで移動させる。
	bool changeIndex(int indexBefore, int indexAfter);
	//指定したエフェクトのインデックスを設定する。エフェクトが存在したらtrueを、しなかったらfalseを返す。
	bool setIndex(const RawEffect& effect, int index);
	//対象のエフェクトが存在するかを調べ、存在した場合はインデックスを、しなかった場合は-1を返す。
	int lookUpEffect(const RawEffect& effect);
	//エフェクトを追加し、そのインデックスを返す。第二引数にfalseを指定すると、無効化された状態で追加する。
	int addEffect(const RawEffect& effect, bool valid = true);

	//有効なエフェクトをベクターで返す。
	std::vector<Effect> getValidEffects();
	//無効なエフェクトをベクターで返す。
	std::vector<Effect> getInvalidEffects();
	//すべてのエフェクトをベクターで返す。
	std::vector<Effect> getAllEffects();

	Effect& operator[](int index);

private:
	std::vector<Effect> effects;

	void sort();
};

