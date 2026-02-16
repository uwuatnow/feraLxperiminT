#pragma once
#include "Game/Globals.h"
#include <vector>
#include <algorithm>

namespace nyaa {

template<typename T>
class Animator
{
public:
	T& target;

	static std::vector<Animator<T>*> GlobalAnimatorsVec;

	Animator(T& target)
		:target(target)
	{
		GlobalAnimatorsVec.push_back(this);
	}

	~Animator()
	{
		auto it = std::find(GlobalAnimatorsVec.begin(), GlobalAnimatorsVec.end(), this);
		if (it != GlobalAnimatorsVec.end()) GlobalAnimatorsVec.erase(it);
	}
};

class FloatAnimator : public Animator<float> {};

}
