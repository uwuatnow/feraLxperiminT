#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Screen/InGameScreen.h"
#include "Game/Kb.h"
#include "Game/Mouse.h"
#include <SFML/Window/Joystick.hpp>
#include "Inventory/Inventory.h"
#include "Inventory/Item.h"
#include "Game/Util.h"
#include "Map/Map.h"

namespace nyaa {

float Actor::ShiftSpeedBoostMultiplier = 3.f;

bool Actor::canUseItems()
{
	if(
		guip_eof == IGS
		
	)
	{
		if (inv->equippedItem)
		{
			return true;
		}
	}
	return false;
}

float Actor::control()
{
	if (IGS->dbox.showing || !G->winFocused || !isControllable)
	{
		return 0.0f;
	}

	float dt = G->frameDeltaMillis / 1000.0f;

	bool wasAnim = (flags & EntFlag_Animating);
	flags &= ~EntFlag_Animating;

	float oldDirAngle = dirAngle;

	auto& ji = G->joystickIndex;
	
	if (canUseItems()
		&& (
			Mouse::LeftFrames
			|| sf::Joystick::isButtonPressed(ji, 1) // Circle button to use equipped item
			|| Kb::IsKeyDown(KB::RControl)
			)
		)
	{
		inv->equippedItem->use();
	}

	auto upb = Kb::IsKeyDown(KB::W);
	auto downb = Kb::IsKeyDown(KB::S);
	auto leftb = Kb::IsKeyDown(KB::A);
	auto rightb = Kb::IsKeyDown(KB::D);

	bool up = upb, down = downb, left = leftb, right = rightb;
	float joyX = sf::Joystick::getAxisPosition(ji, sf::Joystick::Axis::X);
	float joyY = sf::Joystick::getAxisPosition(ji, sf::Joystick::Axis::Y);
	auto deadzone = 30.f;
	if (std::fabs(joyX) < deadzone)
	{
		joyX = 0;
	}
	if (std::fabs(joyY) < deadzone)
	{
		joyY = 0;
	}
	auto joyTotal = std::fabs(joyX + joyY);

	unsigned int all = up + down + left + right;

	if (G->inMethod == InputMethod_Keyboard)
	{
		if (up && down)
		{
			up = down = false;
		}
		if (left && right)
		{
			left = right = false;
		}



		if (up || down || left || right)
		{
			flags |= EntFlag_Animating;
			if (!wasAnim)
			{
				foot = false;
				//std::cout << "left foot\n";
				if (this == IGS->player)
				{
					moveStartPosX = posX;
					moveStartPosY = posY;
					footstepPlayedThisMove = false;
					isSnappingBack = false; // cancel any ongoing snap
				}
			}
		}

		if (up)
		{
			dirAngle = 270;
		}
		if (down)
		{
			dirAngle = 90;
		}
		if (left)
		{
			dirAngle = 90 * 2;
		}
		if (right)
		{
			dirAngle = 0;
		}

		if (up && left)
		{
			dirAngle = (90 * 2) + 45;
		}
		if (up && right)
		{
			dirAngle = (90 * 3) + 45;
		}
		if (down && left)
		{
			dirAngle = 90 + 45;
		}
		if (down && right)
		{
			dirAngle = 45;
		}
	}
	else if (G->inMethod == InputMethod_Controller)
	{
		float angle = Util::RotateTowards(0, 0, joyX, joyY);
		if (joyTotal > 0)
		{
			flags |= EntFlag_Animating;
			dirAngle = angle;
		}
	}

	if (std::fabs(dirAngle - oldDirAngle) == 180.0f) {
		float intermediateAngle = std::fmod(oldDirAngle - 90.0f + 360.0f, 360.0f);
		visualDir = Util::DirFromAngle(intermediateAngle);
		visualTurnTarget = getDir();
		visualTurnTimer->setMillis(500);
	} else {
		visualDir = getDir();
	}

	if (flags & EntFlag_Animating)
	{
		energy -= ((float)energyDelT->millis()) / 205.0f;
		energy = Util::Clamp(energy, 0, 100);
		notMovingT->zero();
		if (wasAnim == false) moveAccelTimer->zero();
	}

	energyDelT->zero();

	float mo =
		Util::Clamp((float)moveAccelTimer->secs() * 4, 0.25f, moveAmountMax * (energy < 2 ? 0.25f : 1.0f))
		* 1.0f;
	
	// Increase speed when left shift is held in debug mode
#if DEBUG
	if (isControllable && this == IGS->player && Kb::IsKeyDown(KB::LShift))
	{
		mo *= ShiftSpeedBoostMultiplier;
	}
#endif
	
	mo *= (G->frameDeltaMillis / (1000 / Game::FPSConstant));

	bool collided = false;

	float collideDist = 8/*  * (inst->frameDeltaMillis / (1000 / Game::FPSConstant)) */;

	if (((flags & EntFlag_Animating) || physicsSlideAmount > 0) && !(flags & EntFlag_UnderAllOtherEnts))
	{
#if DEBUG
		if (!IGS->collisionDisabled)
#endif
		{
			collided = checkCollide(hostMap->collLines, mo, dirAngle, collideDist, nullptr) != nullptr;
		}

		// //check if floor is 0, cant walk on empty floor
		// if (!collided)
		// {
		// 	auto ht = sf::Vector2i{};
		// 	getTileInFront(left ? Direction_Left : right ? Direction_Right : Direction_Default, &ht.x, &ht.y);
		// 	auto vt = sf::Vector2i{};
		// 	getTileInFront(up ? Direction_Up : down ? Direction_Down : Direction_Default, &vt.x, &vt.y);
		// 	MapTile* htp = 0, * vtp = 0;
		// 	hostMap->getTILE(ht.x, ht.y, 0, &htp);
		// 	hostMap->getTILE(vt.x, vt.y, 0, &vtp);
		// 	if (vtp)
		// 	{
		// 		if (up)
		// 		{

		// 		}
		// 		else if (down)
		// 		{

		// 		}
		// 	}
		// 	if (htp)
		// 	{
		// 		if (left)
		// 		{

		// 		}
		// 		else if (right)
		// 		{

		// 		}
		// 	}
		// }


	}

	if (G->inMethod == InputMethod_Keyboard)
	{
		if (!collided && (flags & EntFlag_Animating))
		{
			physicsSlideAmount = mo / dt;
			physicsSlideAngle = dirAngle;
		}
	}
	else if (G->inMethod == InputMethod_Controller)
	{
		if (!collided && (flags & EntFlag_Animating))
		{
			float ax = joyX / 100;
			float ay = joyY / 100;
			float inputMag = std::sqrt(ax * ax + ay * ay);
			physicsSlideAmount = (mo * inputMag) / dt;
			physicsSlideAngle = dirAngle;
		}
	}

	// Jump (Space key or X button)
	if ((Kb::IsKeyDown(KB::Space) || sf::Joystick::isButtonPressed(ji, 0)) && posZ <= 0.0f) jump();

	if (!(flags & EntFlag_Animating) && posZ == 0.0f) physicsSlideAmount = 0;

	// Snap back if no footstep played
	if (this == IGS->player && wasAnim && !(flags & EntFlag_Animating) && !footstepPlayedThisMove && posZ == 0.0f)
	{
		isSnappingBack = true;
		snapBackTargetX = moveStartPosX;
		snapBackTargetY = moveStartPosY;
	}

	return mo;
}

void Actor::jump()
{
	if(energy < 10)
	{
		return;
	}
	// Remove from any on top lists
	for (auto e : hostMap->entities) {
		auto it = std::find(e->entitiesOnTop.begin(), e->entitiesOnTop.end(), this);
		if (it != e->entitiesOnTop.end()) e->entitiesOnTop.erase(it);
	}
	velZ = JumpSpeed;

	energy -= 4;
}

}