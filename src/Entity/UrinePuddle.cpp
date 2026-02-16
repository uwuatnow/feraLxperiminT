#include "UrinePuddle.h"
#include "Game/Util.h"
#include "Entity/Interaction.h"
#include "Game/Game.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "Map/Map.h"
#include "Entity/Actor.h"
#include "Game/Timer.h"

namespace nyaa {

float UrinePuddle::MinimumPissTime = 1.5f;

UrinePuddle::UrinePuddle(Actor* wetter)
	:puddleSpreadTimer(new Timer())
	,wetter(wetter)
	,done(false)
{
	wetter->urinePuddle = this;
	bladderOnPiss = wetter->bladder;
	seconds = Util::ScaleClamped(wetter->bladder, 0, 100, MinimumPissTime, 22.4f);
	PuddleRadiusPx = Util::ScaleClamped(wetter->bladder, 0, 100, 3, 15);
	flags |= EntFlag_UnderAllOtherEnts;
	posX = wetter->posX;
	posY = wetter->posY;
	updateTPos();
	interactions.push_back(new Interaction(this, "Clean up", [](Interaction* in) -> bool
	{
		in->e->flags |= EntFlag_Dead;
		return true;
		/*
		UrinePuddle*up=(UrinePuddle*)in->e;
		up->wetter->urinePuddle=*/
	}));
}

void UrinePuddle::update()
{
	if (done) return;
	if (Util::Dist(wetter->posX, wetter->posY, posX, posY) > 3)
	{
		done = true;
		wetter->isControllable = true;
		auto nup = new UrinePuddle(wetter);
		wetter->hostMap->addEnt(nup);//drops :3
	}
	puddleSpreadTimer->update();
	float rt = seconds - 4.7;
	float time = Util::Clamp(puddleSpreadTimer->secs(), 0, seconds);
	//float tsecs = Easing::ApplyClamped(Ease_None, time - 0.25, 0.25, seconds);
	if (time > rt)
	{
		time = rt;
	}
	if (time < rt)
	{
		wetter->isControllable = false;
		wetter->bladder = bladderOnPiss - Util::Scale(time, 0, rt, 0, 100);
	}
	else if (!done)
	{
		//G->Wet->stop();
		done = true;
		wetter->isControllable = true;
		if (wetter->urinePuddle == this)
		{
			wetter->urinePuddle = nullptr;
		}
	}
	Entity::update();
}

UrinePuddle::~UrinePuddle()
{
	if (wetter && wetter->urinePuddle == this)
	{
		wetter->urinePuddle = nullptr;
	}
	delete puddleSpreadTimer;
}
}