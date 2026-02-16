#pragma once
#include "Game/Globals.h"
#include <SFML/Graphics/Shader.hpp>
#include <list>

namespace nyaa {

class ShaderDef
{
public:
	ShaderDef(std::string vertName, std::string fragName, ShaderProg& shader);

public:
	std::string vertName, fragName;
	ShaderProg& shader;
};

class Shader
{
public:
	Shader(std::string vertName, std::string fragName);
	Shader(std::string bothNames);
	
	virtual ~Shader();

public:
	virtual void update();

public:
	static std::list<ShaderDef> Shaders;

public:
	ShaderProg shader;
};

}