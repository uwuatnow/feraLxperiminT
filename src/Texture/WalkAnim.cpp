#include "WalkAnim.h"
#include "Entity/Actor.h"
#include <cassert>
#include "Game/Game.h"

namespace nyaa {

WalkAnim::WalkAnim(std::string name, Actor* host)
	:host(host)
	,frame_time_ms(125)
	,index(0)
	,frame(0)
	,ping_pong(true)
	,frame_incrementing(true)
	,columns(3)
	,rows(3)
	,stationary_frame(1)
{
	auto loc = std::string(Game::ResDir) + name + ".png";
	if (!tex.loadFromFile(loc)) std::printf("couldn't load png for walkanim at %s\n", loc.c_str());
	else tex.generateMipmap();
}

void WalkAnim::update()
{
	timer.update();
	if ((host->flags & Entity::EntFlag_Animating) || host->physicsSlideAmount > 0.04) {
		if (timer.millis() >= frame_time_ms) {
			frame += frame_incrementing ? 1 : -1;
			if (frame == columns) {
				if (ping_pong) {
					frame_incrementing = false;
					frame = columns - 2;
				}
				else frame = 0;
			}
			if (frame < 0) {
				frame_incrementing = true;
				frame = 1;
			}
			timer.zero();
		}
	}
	else {
		timer.zero();
		frame = stationary_frame;
	}
	assert(frame >= 0 && frame < columns);
}

}