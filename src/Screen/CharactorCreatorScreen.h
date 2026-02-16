#pragma once
#include "Game/Globals.h"
#include "Screen/Screen.h"
#include "Game/SelectionPrompt.h"
#include "Game/Timer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace nyaa {

class CharactorCreatorScreen : public Screen
{
public:
	static CharactorCreatorScreen* Instance;

	enum class OutfitPart : int
	{
		Body,
		Eyes,
		Hair,
		Clothes,
		Tail,
		Outline,
		Count
	};
	
	static const char* OutfitPartNames[];
	
public:
	CharactorCreatorScreen();

	virtual ~CharactorCreatorScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;

	virtual bool onEnter() override;

	virtual bool onLeave() override;

private:
	void updateSels();
	
	void getSelCol(unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA);
	void setSelCol(unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA);

public:
	SelectionPrompt prompt;
	
	SelectionPrompt::Selection& partSel;	
	SelectionPrompt::Selection& redSel;
	SelectionPrompt::Selection& greenSel;
	SelectionPrompt::Selection& blueSel;
	SelectionPrompt::Selection& opacitySel;
	
	int selectedOutfitPart;
	
	// Character preview animation
	Timer walkTimer;
	Timer rotationTimer;
	int currentDirection;
	int walkFrame;
	bool frameIncrementing;
	
	// HSV color picker
	float hue, saturation, value;
	bool draggingHSV;
	int hsvPickerPosX, hsvPickerPosY;
	int hsvPickerSizeX, hsvPickerSizeY;
	sf::Texture hsvTexture;
	sf::Sprite hsvSprite;
	
private:
	void renderCharacterPreview(RendTarget* renderTarget);
	void renderHSVColorPicker(RendTarget* renderTarget);
	void hsvToRgb(float h, float s, float v, unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA);
	void rgbToHsv(unsigned char r, unsigned char g, unsigned char b, float& h, float& s, float& v);
};

}