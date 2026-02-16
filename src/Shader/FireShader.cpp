#include "Shader/FireShader.h"
#include "Game/Game.h"

namespace nyaa {

FireShader::FireShader()
	:Shader("default", "fire")
{
	//FireShader::Ptr fs = FireShader::New();
}

FireShader::~FireShader()
{
}

void FireShader::update()
{
	shader.setUniform("resolution", sf::Vector2f(Game::ScreenWidth, Game::ScreenHeight));
	shader.setUniform("time", (float)clock.getElapsedSeconds());
}

}