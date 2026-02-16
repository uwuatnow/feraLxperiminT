#include "TextureMan.h"
#include "Game/Util.h"

namespace nyaa {

TextureMan* TextureMan::tman = nullptr;

void TextureMan::Init()
{
	tman = new TextureMan();
}

void TextureMan::Cleanup()
{
	delete tman;
	tman = nullptr;
}

TextureMan::Tex::Tex(std::string name)
	:name(name)
{
	if (tex.loadFromFile(Util::GetPNGLoc(this->name)))
		tex.generateMipmap();
}

TextureMan::~TextureMan()
{
	for(auto t : textures)
	{
		delete t;
	}
}

TextureMan::Tex& TextureMan::get(const char* name)
{
	for (auto t : textures)
	{
		if (name == t->name)
		{
			return *t;
		}
	}
	textures.push_back(new Tex(name));
	return *textures.back();
}

}