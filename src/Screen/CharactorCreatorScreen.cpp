#include "Screen/CharactorCreatorScreen.h"
#include "Screen/InGameScreen.h"
#include "Screen/NewGameScreen.h"
#include "Screen/MenuScreen.h"
#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include "Game/Mouse.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <SFML/Window/Mouse.hpp>
#include "Texture/WalkAnim.h"

namespace nyaa {

CharactorCreatorScreen* CharactorCreatorScreen::Instance = nullptr;

const char* CharactorCreatorScreen::OutfitPartNames[] =
{
	"Body",
	"Eyes",
	"Hair",
	"Clothes",
	"Tail",
	"Outline"
};

CharactorCreatorScreen::CharactorCreatorScreen()
	:prompt(
		(float) Game::ScreenWidth - 150,
		Game::ScreenHeight - ((float) Game::ScreenHeight / 2),
		150,
		250
	)
	//all but last prompt selection are generated at runtime
	//use max len string example for proper alignment
	,partSel(
		prompt.add("Outfit part: Outline", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
		{
			if(d == Direction_Center)
			{
				d = Direction_Right;
			}
			auto seldOFPBef = selectedOutfitPart;
			switch(d)
			{
			case Direction_Left:
			{
				//std::cout << "CharactorCreatorScreen::selectedOutfitPart decremented" << std::endl;
				if(selectedOutfitPart > 0) selectedOutfitPart--;
				break;
			}
			case Direction_Right:
			{
				//std::cout << "CharactorCreatorScreen::selectedOutfitPart incremented" << std::endl;
				selectedOutfitPart++;
				break;
			}
			default:
			{
				return SelPResp_Neutral;
				break;
			}
			}
			updateSels();
			return seldOFPBef != selectedOutfitPart ? SelPResp_DidSomething : SelPResp_Neutral;
		})
	)
	,redSel(
		prompt.add("Color red: 255", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
		{
			if(!IGS) return SelPResp_Neutral;
			if(!IGS->player)
			{
				return SelPResp_Neutral;
			}
			if(d == Direction_Center)
			{
				d = Direction_Right;
			}
			sf::Color selCol{};
			getSelCol(&selCol.r, &selCol.g, &selCol.b, &selCol.a);
			switch(d)
			{
			case Direction_Left:
			{
				selCol.r--;
				break;
			}
			case Direction_Right:
			{				
				selCol.r++;
				break;
			}
			default:
			{
				return SelPResp_Neutral;
				break;
			}
			}
			setSelCol(selCol.r, selCol.g, selCol.b, selCol.a);
			updateSels();
			return SelPResp_DidSomething;
		})
	)
	,greenSel(
		prompt.add("Color green: 255", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
		{
			if(!IGS) return SelPResp_Neutral;
			if(!IGS->player)
			{
				return SelPResp_Neutral;
			}
			if(d == Direction_Center)
			{
				d = Direction_Right;
			}
			sf::Color selCol{};
			getSelCol(&selCol.r, &selCol.g, &selCol.b, &selCol.a);
			switch(d)
			{
			case Direction_Left:
			{
				selCol.g--;
				break;
			}
			case Direction_Right:
			{				
				selCol.g++;
				break;
			}
			default:
			{
				return SelPResp_Neutral;
				break;
			}
			}
			setSelCol(selCol.r, selCol.g, selCol.b, selCol.a);
			updateSels();
			return SelPResp_DidSomething;
		})
	)
	,blueSel(
		prompt.add("Color blue: 255", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
		{
			if(!IGS) return SelPResp_Neutral;
			if(!IGS->player)
			{
				return SelPResp_Neutral;
			}
			if(d == Direction_Center)
			{
				d = Direction_Right;
			}
			sf::Color selCol{};
			getSelCol(&selCol.r, &selCol.g, &selCol.b, &selCol.a);
			switch(d)
			{
			case Direction_Left:
			{
				selCol.b--;
				break;
			}
			case Direction_Right:
			{				
				selCol.b++;
				break;
			}
			default:
			{
				return SelPResp_Neutral;
				break;
			}
			}
			setSelCol(selCol.r, selCol.g, selCol.b, selCol.a);
			updateSels();
			return SelPResp_DidSomething;
		})		
	)
	,opacitySel(
		prompt.add("Color opacity: 255", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
		{
			//std::cout << "opacity" << std::endl;
			if(!IGS) return SelPResp_Neutral;
			//std::cout << "opacity 2" << std::endl;
			if(!IGS->player)
			{
				return SelPResp_Neutral;
			}
			
			//std::cout << "opacity 3" << std::endl;
			if(d == Direction_Center)
			{
				d = Direction_Right;
			}
			sf::Color selCol{};
			getSelCol(&selCol.r, &selCol.g, &selCol.b, &selCol.a);
			switch(d)
			{
			case Direction_Left:
			{
				selCol.a--;
				break;
			}
			case Direction_Right:
			{				
				selCol.a++;
				break;
			}
			default:
			{
				return SelPResp_Neutral;
				break;
			}
			}
			setSelCol(selCol.r, selCol.g, selCol.b, selCol.a);
			updateSels();
			//std::cout << "opacity end" << std::endl;
			return SelPResp_DidSomething;
		})
	)
	,selectedOutfitPart(0)
	,currentDirection(0)
	,walkFrame(1)
	,frameIncrementing(true)
	,hue(0.0f)
	,saturation(1.0f)
	,value(1.0f)
	,draggingHSV(false)
	,hsvPickerPosX(Game::ScreenWidth - 200)
	,hsvPickerPosY(50)
	,hsvPickerSizeX(150)
	,hsvPickerSizeY(150)
{
	Instance = this;

	rotationTimer.reset();
	walkTimer.reset();
	redSel.fillColorR = 255; redSel.fillColorG = 0; redSel.fillColorB = 0;
	greenSel.fillColorR = 0; greenSel.fillColorG = 255; greenSel.fillColorB = 0;
	blueSel.fillColorR = 0; blueSel.fillColorG = 0; blueSel.fillColorB = 255;
	
	prompt.add("Finished", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		if(myLastScreen == (Screen*) MenuScreen::Instance)
		{
			((Screen*) NewGameScreen::Instance)->switchTo();
		}
		else if(myLastScreen != nullptr)
		{
			myLastScreen->switchTo();
		}
		return SelPResp_DidSomething;
	});
	
	updateSels();
}

CharactorCreatorScreen::~CharactorCreatorScreen()
{
}

void CharactorCreatorScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	if(!IGS) return;
	assert(IGS->player);
	if(IGS->player)
	{
		// Update animation timers
		walkTimer.update();
		rotationTimer.update();
		
		if(walkTimer.millis() >= 250) {
			walkFrame += frameIncrementing ? 1 : -1;
			if(walkFrame >= 3) {
				frameIncrementing = false;
				walkFrame = 1;
			}
			if(walkFrame < 0) {
				frameIncrementing = true;
				walkFrame = 1;
			}
			walkTimer.zero();
		}
		
		if(rotationTimer.secs() > 2.0f) {
			currentDirection = (currentDirection + 1) % 4;
			rotationTimer.reset();
			walkTimer.zero();
		}
		
		renderCharacterPreview(renderTarget);
		renderHSVColorPicker(renderTarget);
		
		prompt.update();
		prompt.render(renderTarget);
	}
}

bool CharactorCreatorScreen::onEnter()
{
	// Create HSV color picker texture
	hsvTexture.create(hsvPickerSizeX - 20, hsvPickerSizeY - 40);
	sf::Uint8* pixels = new sf::Uint8[(hsvPickerSizeX - 20) * (hsvPickerSizeY - 40) * 4];
	
	for(int y = 0; y < hsvPickerSizeY - 40; y++) {
		for(int x = 0; x < hsvPickerSizeX - 20; x++) {
			float h = (float)x / (hsvPickerSizeX - 20) * 360.0f;
			float s = 1.0f - (float)y / (hsvPickerSizeY - 40);
			sf::Color color{};
			hsvToRgb(h, s, 1.0f, &color.r, &color.g, &color.b, &color.a);
			
			int index = (y * (hsvPickerSizeX - 20) + x) * 4;
			pixels[index] = color.r;
			pixels[index + 1] = color.g;
			pixels[index + 2] = color.b;
			pixels[index + 3] = 255;
		}
	}
	
	hsvTexture.update(pixels);
	hsvSprite.setTexture(hsvTexture);
	hsvSprite.setPosition(hsvPickerPosX + 10, hsvPickerPosY + 30);
	
	delete[] pixels;
	return true;
}

bool CharactorCreatorScreen::onLeave()
{
	return true;
}

void CharactorCreatorScreen::updateSels()
{
	if(!IGS) return;
	//std::cout << "CharactorCreatorScreen::updateSels called" << std::endl;
	
	partSel.setText(
		Util::Format("Outfit part: %s",
			OutfitPartNames[(int)Util::Clamp(selectedOutfitPart % (int)OutfitPart::Count, 0, (int)OutfitPart::Count - 1)]
		)
	);
	assert(IGS->player);
	sf::Color selCol{};
	getSelCol(&selCol.r, &selCol.g, &selCol.b, &selCol.a);
	if(IGS->player)
	{
		redSel.setText(
			Util::Format("Color red: %d",
				selCol.r
			)
		);
		greenSel.setText(
			Util::Format("Color green: %d",
				selCol.g
			)
		);
		blueSel.setText(
			Util::Format("Color blue: %d",
				selCol.b
			)
		);
		opacitySel.setText(
			Util::Format("Color opacity: %d",
				selCol.a
			)
		);
	}
}

void CharactorCreatorScreen::getSelCol(unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA)
{
	if(!IGS || !IGS->player) {
		if(outR) *outR = 255; 
		if(outG) *outG = 255; 
		if(outB) *outB = 255; 
		if(outA) *outA = 255;
		return;
	}
	
	switch((OutfitPart)(selectedOutfitPart % ((int)OutfitPart::Count)))
	{
	case OutfitPart::Body: {
		if(outR) *outR = IGS->player->bodyR;
		if(outG) *outG = IGS->player->bodyG;
		if(outB) *outB = IGS->player->bodyB;
		if(outA) *outA = IGS->player->bodyA;
		return;
	}
	case OutfitPart::Eyes: {
		if(outR) *outR = IGS->player->eyeR;
		if(outG) *outG = IGS->player->eyeG;
		if(outB) *outB = IGS->player->eyeB;
		if(outA) *outA = IGS->player->eyeA;
		return;
	}
	case OutfitPart::Hair: {
		if(outR) *outR = IGS->player->hairR;
		if(outG) *outG = IGS->player->hairG;
		if(outB) *outB = IGS->player->hairB;
		if(outA) *outA = IGS->player->hairA;
		return;
	}
	case OutfitPart::Clothes: {
		if(outR) *outR = IGS->player->clothesR;
		if(outG) *outG = IGS->player->clothesG;
		if(outB) *outB = IGS->player->clothesB;
		if(outA) *outA = IGS->player->clothesA;
		return;
	}
	case OutfitPart::Tail: {
		if(outR) *outR = IGS->player->tailR;
		if(outG) *outG = IGS->player->tailG;
		if(outB) *outB = IGS->player->tailB;
		if(outA) *outA = IGS->player->tailA;
		return;
	}
	case OutfitPart::Outline: {
		if(outR) *outR = IGS->player->outlineR;
		if(outG) *outG = IGS->player->outlineG;
		if(outB) *outB = IGS->player->outlineB;
		if(outA) *outA = IGS->player->outlineA;
		return;
	}
	default: {
		if(outR) *outR = 255;
		if(outG) *outG = 255;
		if(outB) *outB = 255;
		if(outA) *outA = 255;
		return;
	}
	}
}

void CharactorCreatorScreen::setSelCol(unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA)
{
	if(!IGS || !IGS->player) return;
	
	switch((OutfitPart)(selectedOutfitPart % ((int)OutfitPart::Count)))
	{
	case OutfitPart::Body:
		IGS->player->bodyR = colorR;
		IGS->player->bodyG = colorG;
		IGS->player->bodyB = colorB;
		IGS->player->bodyA = colorA;
		break;
	case OutfitPart::Eyes:
		IGS->player->eyeR = colorR;
		IGS->player->eyeG = colorG;
		IGS->player->eyeB = colorB;
		IGS->player->eyeA = colorA;
		break;
	case OutfitPart::Hair:
		IGS->player->hairR = colorR;
		IGS->player->hairG = colorG;
		IGS->player->hairB = colorB;
		IGS->player->hairA = colorA;
		break;
	case OutfitPart::Clothes:
		IGS->player->clothesR = colorR;
		IGS->player->clothesG = colorG;
		IGS->player->clothesB = colorB;
		IGS->player->clothesA = colorA;
		break;
	case OutfitPart::Tail:
		IGS->player->tailR = colorR;
		IGS->player->tailG = colorG;
		IGS->player->tailB = colorB;
		IGS->player->tailA = colorA;
		break;
	case OutfitPart::Outline:
		IGS->player->outlineR = colorR;
		IGS->player->outlineG = colorG;
		IGS->player->outlineB = colorB;
		IGS->player->outlineA = colorA;
		break;
	default: {
		break;
	}
	}
}

void CharactorCreatorScreen::renderCharacterPreview(RendTarget* renderTarget)
{
	if(!IGS || !IGS->player) return;
	
	// Character preview position (center-left of screen)
	sf::Vector2f previewPos(Game::ScreenWidth * 0.3f, Game::ScreenHeight * 0.9f);
	
	// Calculate scale to fit screen height
	float scale = 3.0f;
	if(IGS->player->body->tex.getSize().y > 0 && IGS->player->body->rows > 0) {
		int frameHeight = IGS->player->body->tex.getSize().y / IGS->player->body->rows;
		scale = (Game::ScreenHeight * 0.8f) / frameHeight;
	}
	
	// Set player direction and frame for animation
	IGS->player->dirAngle = Util::AngleFromDir((Direction)currentDirection);
	
	// Render each layer with animation frame
	auto renderLayer = [&](WalkAnim& anim, sf::Color color) {
		if(anim.tex.getSize().x > 0 && anim.columns > 0 && anim.rows > 0) {
			int frameWidth = anim.tex.getSize().x / anim.columns;
			int frameHeight = anim.tex.getSize().y / anim.rows;
			
			int row = std::min(currentDirection, anim.rows - 1);
			bool flip = (currentDirection == 3); // Right direction
			if(flip && anim.rows > 2) row = 2; // Use left sprites flipped
			
			int safeWalkFrame = std::min(walkFrame, anim.columns - 1);
			int frameX = safeWalkFrame * frameWidth;
			int frameY = row * frameHeight;
			
			sf::Sprite sprite;
			sprite.setTexture(anim.tex);
			sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
			sprite.setColor(color);
			sprite.setScale(scale, scale);
			sprite.setOrigin(frameWidth * 0.5f, frameHeight);
			sprite.setPosition(previewPos);
			
			if(flip) {
				sprite.setScale(-scale, scale);
			}
			
			renderTarget->draw(sprite);
		}
	};
	
	// Render layers in order
	renderLayer(*IGS->player->body, sf::Color(IGS->player->bodyR, IGS->player->bodyG, IGS->player->bodyB, IGS->player->bodyA));
	renderLayer(*IGS->player->eyes, sf::Color(IGS->player->eyeR, IGS->player->eyeG, IGS->player->eyeB, IGS->player->eyeA));
	renderLayer(*IGS->player->hair, sf::Color(IGS->player->hairR, IGS->player->hairG, IGS->player->hairB, IGS->player->hairA));
	renderLayer(*IGS->player->clothes, sf::Color(IGS->player->clothesR, IGS->player->clothesG, IGS->player->clothesB, IGS->player->clothesA));
	renderLayer(*IGS->player->tail, sf::Color(IGS->player->tailR, IGS->player->tailG, IGS->player->tailB, IGS->player->tailA));
	renderLayer(*IGS->player->outline, sf::Color(IGS->player->outlineR, IGS->player->outlineG, IGS->player->outlineB, IGS->player->outlineA));
}

void CharactorCreatorScreen::renderHSVColorPicker(RendTarget* renderTarget)
{
	// HSV color picker background
	sf::RectangleShape pickerBg(sf::Vector2f(hsvPickerSizeX, hsvPickerSizeY));
	pickerBg.setPosition(hsvPickerPosX, hsvPickerPosY);
	pickerBg.setFillColor(sf::Color(50, 50, 50, 200));
	pickerBg.setOutlineColor(sf::Color::White);
	pickerBg.setOutlineThickness(2);
	renderTarget->draw(pickerBg);
	
	// Draw HSV texture
	renderTarget->draw(hsvSprite);
	
	// Handle mouse input
	sf::Vector2i mousePos = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
	if(Mouse::LeftFrames) {
		if(mousePos.x >= hsvPickerPosX + 10 && mousePos.x < hsvPickerPosX + hsvPickerSizeX - 10 &&
		   mousePos.y >= hsvPickerPosY + 30 && mousePos.y < hsvPickerPosY + hsvPickerSizeY - 10) {
			
			float h = (float)(mousePos.x - hsvPickerPosX - 10) / (hsvPickerSizeX - 20) * 360.0f;
			float s = 1.0f - (float)(mousePos.y - hsvPickerPosY - 30) / (hsvPickerSizeY - 40);
			
			sf::Color newColor{};
			hsvToRgb(h, s, 1.0f, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
			setSelCol(newColor.r, newColor.g, newColor.b, newColor.a);
			updateSels();
		}
	}
	
	// Draw current color preview
	sf::Color currentColor{};
	getSelCol(&currentColor.r, &currentColor.g, &currentColor.b, &currentColor.a);
	sf::RectangleShape colorPreview(sf::Vector2f(hsvPickerSizeX - 20, 20));
	colorPreview.setPosition(hsvPickerPosX + 10, hsvPickerPosY + 5);
	colorPreview.setFillColor(currentColor);
	colorPreview.setOutlineColor(sf::Color::White);
	colorPreview.setOutlineThickness(1);
	renderTarget->draw(colorPreview);
}

void CharactorCreatorScreen::hsvToRgb(float h, float s, float v, unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA)
{
	float c = v * s;
	float x = c * (1 - abs(fmod(h / 60.0f, 2) - 1));
	float m = v - c;
	
	float r, g, b;
	if(h >= 0 && h < 60) {
		r = c; g = x; b = 0;
	} else if(h >= 60 && h < 120) {
		r = x; g = c; b = 0;
	} else if(h >= 120 && h < 180) {
		r = 0; g = c; b = x;
	} else if(h >= 180 && h < 240) {
		r = 0; g = x; b = c;
	} else if(h >= 240 && h < 300) {
		r = x; g = 0; b = c;
	} else {
		r = c; g = 0; b = x;
	}

	if(outR) *outR = (r + m) * 255;
	if(outG) *outG = (g + m) * 255;
	if(outB) *outB = (b + m) * 255;
	if(outA) *outA = 255;
}

void CharactorCreatorScreen::rgbToHsv(unsigned char r_, unsigned char g_, unsigned char b_, float& h, float& s, float& v)
{
	float r = r_ / 255.0f;
	float g = g_ / 255.0f;
	float b = b_ / 255.0f;
	
	float max = std::max({r, g, b});
	float min = std::min({r, g, b});
	float delta = max - min;
	
	v = max;
	s = (max == 0) ? 0 : delta / max;
	
	if(delta == 0) {
		h = 0;
	} else if(max == r) {
		h = 60 * fmod((g - b) / delta, 6);
	} else if(max == g) {
		h = 60 * ((b - r) / delta + 2);
	} else {
		h = 60 * ((r - g) / delta + 4);
	}
	
	if(h < 0) h += 360;
}

}