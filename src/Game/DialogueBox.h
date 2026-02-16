#pragma once
#include "Game/Globals.h"

#include <list>
#include <string>
#include <functional>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Game/Timer.h"
#include "Game/GuiWidget.h"

namespace nyaa {

class Actor;

class DialogueBox : public GuiWidget
{
public:
	class Msg;
	typedef std::function<void(Msg&)> DBPostMsgCback;
	typedef std::function<void()> DBCallback;
	//typedef void(*PostMsgCback)(Msg&);
	class Msg
	{
	public:
		Msg(DialogueBox* host, Actor* from, std::string msg, DBPostMsgCback pmCb = nullptr);
		
		//Msg(DialogueBox* host, Actor* from, std::string msg, sol::function pmCbLUA);

	public:
		void update();
		
		void render(float posX, float posY, char opacity = CHAR_MAX);
		
		bool isDone();
		
		bool scroll(); // returns true when it will be at the bottom

	public:
		//sol::function pmCbLUA;
		DialogueBox* host;
		Actor* actor;
		DBPostMsgCback pmCb;
		sf::Text msg;
		size_t scrollLnY;
		size_t lines;
		Timer scrollTimer;
		float scrollTime;
		bool scrollable; // if > 3 lines
	};

public:	
	DialogueBox();
	
	~DialogueBox();
	
public:
	void resize();
	
	void updateEndText();
	
	void clear();
	
	void add(Actor* from, std::string msg, DBPostMsgCback pmCb = nullptr);
	
	//void add(Actor* from, std::string msg, sol::function pmCbLUA);
	
	void show();
	
	void update();
	
	void render(RendTarget* renderTarget);
	
	void next();

public:
	sf::Text endText;
	//typedef void(*Callback)(void);
	DBCallback closeCallback;
	//sol::function LUAcloseCallback;
	unsigned int curMsg;
	bool showing;
	std::list<Msg> msgs;
	sf::RectangleShape bg;
	sf::RenderTexture msgsTex;
	sf::Sprite texSp;
	Timer slideTimer;
	Timer boxFadeTimer;
	float slideTime;
	float fadeMs;
};

}