#include "Shader/Shader.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include <cassert>
#include <algorithm>

namespace nyaa {

ShaderDef::ShaderDef(std::string vertName, std::string fragName, ShaderProg& shader)
	:vertName(Util::Format("%s" "%s.vs",   Game::ResDir, vertName.c_str()))
	,fragName(Util::Format("%s" "%s.frag", Game::ResDir, fragName.c_str()))
	,shader(shader)
{
	bool loaded = shader.loadFromFile(this->vertName, this->fragName);
	assert(loaded);
}

std::list<ShaderDef> Shader::Shaders;

Shader::Shader(std::string vertName, std::string fragName)
{
	Shaders.push_back(ShaderDef(vertName, fragName, shader));
}

Shader::Shader(std::string bothNames)
{
	Shaders.push_back(ShaderDef(bothNames, bothNames, shader));
}

Shader::~Shader()
{
	auto it = std::find_if(Shaders.begin(), Shaders.end(), 
		[this](const ShaderDef& def) { return &def.shader == &shader; });
	if (it != Shaders.end()) {
		Shaders.erase(it);
	}
}

void Shader::update()
{
}

}