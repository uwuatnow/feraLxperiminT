#include "Animation.h"
#include "Util.h"
#include <cassert>

namespace nyaa {

Anim::Anim(float lengthMilliseconds, float startTimeMilliseconds, bool loop)
	:lengthMilliseconds(lengthMilliseconds)
	,currentTimeMilliseconds(startTimeMilliseconds)
	,loop(loop)
	,state(State_Paused)
	,currentFrameIndex(0)
{
	
}

Anim::Kf& Anim::add()
{
	frames.emplace_back();
	return frames.back();
}

Anim::Kf& Anim::operator[](unsigned int index)
{
	return frames.at(index);
}

Anim::Kf Anim::between()
{
	if (frames.empty())
	{
		return Kf();
	}
	Kf& ckf = frames.at(currentFrameIndex);
	if (currentFrameIndex + 1 < frames.size())
	{
		Kf kf;
		float ctms = Easing::Apply(ckf.easing, currentTimeMilliseconds, 0, lengthMilliseconds, lengthMilliseconds);
		Kf& nkf = frames.at(currentFrameIndex + 1);
		float cKfStartMs = ckf.timeMilliseconds;
		float nKfStatMs = nkf.timeMilliseconds;
		assert(cKfStartMs <= nKfStatMs);
		kf.posX = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.posX, nkf.posX);
		kf.posY = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.posY, nkf.posY);
		kf.alpha = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.alpha, nkf.alpha);
		kf.rotation = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.rotation, nkf.rotation);
		kf.colR = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.colR, nkf.colR);
		kf.colG = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.colG, nkf.colG);
		kf.colB = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.colB, nkf.colB);
		kf.colA = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.colA, nkf.colA);
		kf.rectLeft = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.rectLeft, nkf.rectLeft);
		kf.rectTop = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.rectTop, nkf.rectTop);
		kf.rectWidth = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.rectWidth, nkf.rectWidth);
		kf.rectHeight = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.rectHeight, nkf.rectHeight);
		kf.scaleX = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.scaleX, nkf.scaleX);
		kf.scaleY = Util::Scale(ctms, cKfStartMs, nKfStatMs, ckf.scaleY, nkf.scaleY);
		return kf;
	}
	return frames.back();
}

void Anim::update()
{
	if (state == State_Playing)
	{
		currentTimeMilliseconds += clock.getElapsedMicroseconds() / 1000.0f;
		//printf("%.2f\n", currentTimeMilliseconds);
		clock.restart();

		if (currentTimeMilliseconds > lengthMilliseconds)
		{
			if (!loop) stop();
			else restart();
		}

		int idx = 0;
		for (auto& kf : frames)
		{
			bool bef = kf.visited;
			kf.visited = currentTimeMilliseconds >= kf.timeMilliseconds;
			if (!bef && kf.visited)
			{
				currentFrameIndex = idx;
				break;
			}
			++idx;
		}
	}
}

void Anim::pause()
{
	state = State_Paused;
}

void Anim::restart()
{
	restart2(0);
}

void Anim::restart2(float atMillis)
{
	clock.restart();
	currentTimeMilliseconds = atMillis;
	state = State_Playing;
	
	for (auto& kf : frames)
	{
		kf.visited = false;
	}
}

void Anim::stop()
{
	state = State_Done;
}

Anim::Kf::Kf()
	:posX(0)
	,posY(0)
	,scaleX(0)
	,scaleY(0)
	,rectLeft(0)
	,rectTop(0)
	,rectWidth(0)
	,rectHeight(0)
	,rotation(0)
	,colR(255)
	,colG(255)
	,colB(255)
	,colA(255)
	,alpha(255)
	,timeMilliseconds(0)
	,visited(false)
	,easing(Ease_OutBounce)
{
	
}

}