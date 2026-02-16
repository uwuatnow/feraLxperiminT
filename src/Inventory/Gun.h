#pragma once
#include "Game/Globals.h"
#include "Inventory/Item.h"
#include "Game/Timer.h"

namespace nyaa {

enum class GunType {
	Pistol,
	Rifle
};

class Gun : public Item
{
protected:
	Gun(const std::string& name, const std::string& desc, const std::string& texName, GunType type);
	
public:
	static Gun* Create(GunType type);
	
public:
	virtual void use() override;
	virtual void equippedUpdate() override;
	
protected:
	virtual void shootBullet();
	virtual int getFireRate() const = 0;
	virtual float getRecoil() const = 0;
	virtual int getDamage() const = 0;

public:
	GunType gunType;
	Timer invervalTimer;
	Timer recoilT;
	float gunAngle;
};

class Pistol : public Gun
{
public:
	Pistol();

protected:
	virtual int getFireRate() const override { return 80; }
	virtual float getRecoil() const override { return 40.0f; }
	virtual int getDamage() const override { return 3; }
};

class Rifle : public Gun
{
public:
	Rifle();

protected:
	virtual int getFireRate() const override { return 120; }
	virtual float getRecoil() const override { return 60.0f; }
	virtual int getDamage() const override { return 10; }
};

}