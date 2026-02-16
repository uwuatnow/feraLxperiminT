#pragma once
#include "Game/Globals.h"
#include <string>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace nyaa {

class TextureMan
{
public:
	static TextureMan* tman;
	
	static void Init();
	static void Cleanup();

	class Tex
	{
	public:
		Tex(std::string name);
		
	public:
		std::string name;
		sf::Texture tex;
	};

public:
	~TextureMan();

public:
	Tex& get(const char* name);
	static Tex& Get(const char* name) { return tman->get(name); }

public:
	std::vector<Tex*> textures;
};


}