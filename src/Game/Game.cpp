#include "Game/Game.h"
#include "Game/Util.h"
#include "Shader/Shader.h"
#include "Game/EventScheduler.h"
#include "Game/Controller.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include "Map/Map.h"
#include "Screen/BlackoutScreen.h"
#include "Screen/InGameScreen.h"
#include "Screen/MissionSelectDebugScreen.h"
#include "Entity/Actor.h"
#include "Screen/ExitScreen.h"
#include "Screen/TileSelectScreen.h"
#include "Screen/InventoryScreen.h"
#include "Screen/EditTextScreen.h"
#include "Screen/PortalEditScreen.h"
#include "Map/MapFactory.h"
#include "Screen/InitMissionScreen.h"
#include "Screen/MissionFailedScreen.h"
#include "Screen/PauseScreen.h"
#include "Screen/MapSwitchScreen.h"
#include "Screen/MenuScreen.h"
#include "Screen/NewGameScreen.h"
#include "Screen/OptionsScreen.h"
#include "Screen/CharactorCreatorScreen.h"
#include "Screen/LoadGameScreen.h"
#include "Screen/PCScreen.h"
#include "Screen/MapScreen.h"
#include "Shader/TVStaticShader.h"
#include "Shader/OpenGLComputeFunctions.h"
#include "Game/Fonts.h"
#include "Texture/TextureMan.h"
#include "Game/ProceduralSprite.h"
#include "Game/GameSettings.h"
#include "CrashHandler.h"
#include <SFML/Window/Joystick.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <cstring>
#include <cassert>
#include "Mission/DevTestMission.h"
#include "Shader/TVStaticShader.h"
// #include "Thonk/FLC.h"

// #define M_PI 3.14159265358979323846
// static void testFLCLibrary()
// {
//     std::cout << "=== FLC Library Test (lossy) ===" << std::endl;

//     // Create FLC instance
//     flc::FLC flc;

//     // Create test data - a simple sine wave pattern
//     const int dataSize = 1024;
//     std::vector<float> originalData(dataSize);

//     // Generate test data: sine wave with some noise
//     for (int i = 0; i < dataSize; ++i) {
//         float t = static_cast<float>(i) / dataSize;
//         originalData[i] = sin(t * 2.0 * M_PI) + 0.1 * sin(t * 20.0 * M_PI);
//     }

//     std::cout << "Original data size: " << dataSize << " floats (" << dataSize * sizeof(float) << " bytes)" << std::endl;

//     // Test Fibonacci binary compression
//     std::vector<uint8_t> compressedBytes = flc.compress_to_fib_binary(originalData, 1.0f);

//     // Calculate compressed size
//     size_t compressedSize = compressedBytes.size();
//     std::cout << "Compressed data size: " << compressedSize << " bytes" << std::endl;

//     // Calculate compression ratio
//     float compressionRatio = static_cast<float>(dataSize * sizeof(float)) / compressedSize;
//     std::cout << "Compression ratio: " << compressionRatio << ":1" << std::endl;

//     // Test decompression
//     std::vector<float> decompressedData = flc.decompress_from_fib_binary(compressedBytes);

//     float maxError = 0.0f;
//     float totalError = 0.0f;

//     for (size_t i = 0; i < originalData.size(); ++i) {
//         float error = std::abs(originalData[i] - decompressedData[i]);
//         maxError = std::max(maxError, error);
//         totalError += error;
//     }

//     std::cout << "Max error: " << maxError << std::endl;
//     std::cout << "Average error: " << (totalError / dataSize) << std::endl;

//     // Test DCT/IDCT round trip without quantization
//     std::vector<float> dct_coeffs = flc.dct(originalData);
//     std::vector<float> reconstructed = flc.idct(dct_coeffs);

//     float max_idct_error = 0.0f;
//     float total_idct_error = 0.0f;
//     for (size_t i = 0; i < originalData.size(); ++i) {
//         float error = std::abs(originalData[i] - reconstructed[i]);
//         max_idct_error = std::max(max_idct_error, error);
//         total_idct_error += error;
//     }
//     std::cout << "DCT/IDCT round trip - Max error: " << max_idct_error << ", Average error: " << (total_idct_error / dataSize) << std::endl;

//     std::cout << "=== FLC Test Complete ===" << std::endl << std::endl;
// }

int main()
{
    //testFLCLibrary();
    nyaa::Util::PrintLnFormat("|starting nyaa engine 2026|");
    nyaa::Game g{};
    return 0;
}

namespace nyaa {

Game* G = nullptr;

const char* Game::Title = "feraL_xperiminT < ALPHA_BUILD >";
float Game::FPSConstant = 60.0f;
const char* Game::ResDir = "res/";
int Game::AspectRatioL = 4;
int Game::AspectRatioR = 3;
int Game::ScreenWidth = 640;
int Game::ScreenHeight = 480; //SH*AspectRatioL/AspectRatioR=SW

//

// #if DEBUG
// ProceduralSprite* g_testProcSprite = nullptr;
// #endif

Game::Game()
	:winResized(true)
	,winFocused(true)
	,isFullscreen(false)
	,fadeState(Fade_Done)
	,framesPassed(0)
	,curScreenFramesPassed(0)
	,fpsLimit(FPSConstant)
	,frameDeltaMillis(1000 / fpsLimit)
	,lastTimeFps(0)
	,framesPerSecond(0)
	,inMethod(InputMethod_Keyboard)
	,joystickIndex(0)
	,win(nullptr)
	,gameWinRendTex(nullptr)
	,selectedSaveSlot(0)
{
	G = this;

	// Install crash handler early to catch any initialization crashes
	//CrashHandler::install(); //WE HAVE DEBUGGER NOW LOL

	srand(time(NULL));

	EventScheduler::GlobalScheduler = new EventScheduler();

	Kb::Init();

	Util::MakeDir("./saves/");

	GameSettings::GlobalSettings = new GameSettings();

	sf::ContextSettings settings;
	settings.majorVersion = 4;
	settings.minorVersion = 3;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 8;
	win = new sf::RenderWindow(sf::VideoMode(ScreenWidth, ScreenHeight), Title, sf::Style::Default, settings);
	win->setActive(true);

	// Initialize OpenGL compute functions after context is created
	InitializeOpenGLComputeFunctions();

	gameWinRendTex = new sf::RenderTexture();
	gameWinRendTex->create(ScreenWidth, ScreenHeight);
	gameWinRendTex->setSmooth(true);

	win->setKeyRepeatEnabled(false);
	win->setJoystickThreshold(30);
	Util::MakeDir(Map::MapsDir);

	Fonts::Init();
	TextureMan::Init();

	Sfx::CursorMove = Sfx::Engine.add("17", false);
	Sfx::CursorSel = Sfx::Engine.add("1");

	Sfx::Shoot = Sfx::Engine.add("shoot");
	Sfx::WallHit = Sfx::Engine.add("wallhit");
	Sfx::Hurt = Sfx::Engine.add("hurt");
	Sfx::MissionFailed = Sfx::Engine.add("9");
	Sfx::Wet = Sfx::Engine.add("wet", false);
	Sfx::Open = Sfx::Engine.add("15", false);
	Sfx::Close = Sfx::Engine.add("4", false);
	Sfx::Pick = Sfx::Engine.add("17", false);
	Sfx::StoneLeftFoot = Sfx::Engine.add("left_foot_stone");
	Sfx::StoneRightFoot = Sfx::Engine.add("right_foot_stone");
	Sfx::GravelLeftFoot = Sfx::Engine.add("left_foot_gravel");
	Sfx::GravelRightFoot = Sfx::Engine.add("right_foot_gravel");
	Sfx::CarDoorOpen = Sfx::Engine.add("cardoor_open");
	Sfx::CarDoorClose = Sfx::Engine.add("cardoor_close", false);
	Sfx::CarGearShift = Sfx::Engine.add("shift", false);
	Sfx::CarDrift = Sfx::Engine.add("drift", false);
	Sfx::GunRattle1 = Sfx::Engine.add("gun_rattle1");
	Sfx::GunRattle2 = Sfx::Engine.add("gun_rattle2");
	Sfx::GunRattle3 = Sfx::Engine.add("gun_rattle3");
	Sfx::GunRattle4 = Sfx::Engine.add("gun_rattle4");
	Sfx::Cough1 = Sfx::Engine.add("Cough1");
	Sfx::Cough2 = Sfx::Engine.add("Cough2");
	Sfx::Cough3 = Sfx::Engine.add("Cough3");
	Sfx::Cough4 = Sfx::Engine.add("Cough4");
	Sfx::Cough5 = Sfx::Engine.add("Cough5");
	Sfx::Cough6 = Sfx::Engine.add("Cough6");
	Sfx::Cough7 = Sfx::Engine.add("Cough7");
	Sfx::Cough8 = Sfx::Engine.add("Cough8");
	Sfx::Cough9 = Sfx::Engine.add("Cough9");
	Sfx::Cough10 = Sfx::Engine.add("Cough10");
	Sfx::Cough11 = Sfx::Engine.add("Cough11");
	Sfx::Cough12 = Sfx::Engine.add("Cough12");
	Sfx::Cough13 = Sfx::Engine.add("Cough13");
	Sfx::Cough14 = Sfx::Engine.add("Cough14");
	Sfx::Cough15 = Sfx::Engine.add("Cough15");
	Sfx::Cough16 = Sfx::Engine.add("Cough16");
	Sfx::Cough17 = Sfx::Engine.add("Cough17");
	Sfx::Cough18 = Sfx::Engine.add("Cough18");
	Sfx::Cough19 = Sfx::Engine.add("Cough19");

// #if DEBUG
// 	g_testProcSprite = new ProceduralSprite(32, 32);
// #endif

	MenuScreen::Instance = new MenuScreen();
#if DEBUG
	ReallocInGameScreen();
#endif
	NewGameScreen::Instance = new NewGameScreen();
	new ExitScreen();
	new MapSwitchScreen();
	new PauseScreen();
	new MissionFailedScreen();
	new InitMissionScreen();
	new PortalEditScreen();
	new EditTextScreen();
	new InventoryScreen();
	new BlackoutScreen();
	new OptionsScreen();
	new CharactorCreatorScreen();
	new PCScreen();
	new MapScreen();
#if DEBUG
	missionSelectScreen = new MissionSelectDebugScreen();
	tileSelectScreen = new TileSelectScreen();

	IGS->setMission(new DevTestMission());
	InitMissionScreen::OverrideReturnScreen = IGS;
	InitMissionScreen::Instance->switchTo();
#endif
	new LoadGameScreen();

	// Initialize TV static shader for screen transitions
	tvStaticShader = new TVStaticShader();

	Screen::LastScreen = Screen::ToScreen = Screen::CurrentScreen =
#if DEBUG
		//missionSelectScreen
		IGS
#else
		MenuScreen::Instance
#endif
		;

	for(unsigned int i = 0; i < sf::Joystick::ButtonCount; ++i) 
	{
		Controller::HeldMillis[(Btn)i] = 0.0f;
		Controller::BtnFrames[(Btn)i] = 0;
		Controller::BtnRel[(Btn)i] = false;
	}

	//fireShader = FireShader::New();

	doLoop();
}

Game::~Game()
{
// #if DEBUG
// 	delete g_testProcSprite;
// #endif
	Fonts::Cleanup();
	TextureMan::Cleanup();
	delete NewGameScreen::Instance;
	delete LoadGameScreen::Instance;
	delete ExitScreen::Instance;
	delete MapSwitchScreen::Instance;
	delete PauseScreen::Instance;
	delete MissionFailedScreen::Instance;
	delete InitMissionScreen::Instance;
	delete PortalEditScreen::Instance;
	delete EditTextScreen::Instance;
	delete InventoryScreen::Instance;
	delete BlackoutScreen::Instance;
	delete OptionsScreen::Instance;
	delete CharactorCreatorScreen::Instance;
	delete PCScreen::Instance;
	delete MapScreen::Instance;
#if DEBUG
	delete missionSelectScreen;
	delete tileSelectScreen;
#endif
	delete IGS;
	delete EventScheduler::GlobalScheduler;
	delete GameSettings::GlobalSettings;
	delete tvStaticShader;
	delete win;
	delete gameWinRendTex;
}

void Game::ReallocInGameScreen()
{
	if(IGS)
	{
		delete IGS;
		IGS = nullptr;
	}
#if DEBUG
	sheets.clear();
#endif
	Shader::Shaders.clear();
	IGS = new InGameScreen(selectedSaveSlot);
	if(PCScreen::Instance)
		PCScreen::Instance->accessedFrom = nullptr;
}

void Game::doEvents()
{
	memset(Kb::KeyReleased, 0, sizeof(Kb::KeyReleased));
	Kb::LastEnteredText = "";
	sf::Event e;
	while (win->pollEvent(e))
	{
		auto type = e.type;
		if (type == sf::Event::Closed)
		{
			close();
		}
		if (type == sf::Event::Resized)
		{
			winResized = true;
			win->setSize({win->getSize().y * Game::AspectRatioL / Game::AspectRatioR, win->getSize().y});
			win->clear(sf::Color::Black);
			gameWinRendTex->clear(sf::Color::Black);
			IGS->rendTex.clear(sf::Color::Black);
		}
		if (type == sf::Event::GainedFocus)
		{
			memset(Kb::KeyHeldFrames, 0, sizeof(Kb::KeyHeldFrames));
			memset(Kb::KeyReleased, 0, sizeof(Kb::KeyReleased));
			for(unsigned int i = 0; i < KB::KeyCount; ++i)
			{
				Kb::KeysHeldMillis[i] = 0;
			}
			winFocused = true;
#if DEBUG
			for(auto s : sheets)
			{
				s->reload();
			}
			
			for(auto& s : Shader::Shaders)
			{
				s.shader.loadFromFile(s.vertName, s.fragName);
			}
#endif
		}
		if (type == sf::Event::LostFocus)
		{
			winFocused = false;
#if DEBUG
			if (IGS)
			{
				for(auto m : IGS->mapFactory->maps)
				{
					m->save();
					m->unloadAllChunks();
				}
			}
			GameSettings::GlobalSettings->save();
#endif
		}
		if (type == sf::Event::MouseMoved && (fadeState == Fade_Done && winFocused))
		{
			Mouse::Moved = true;
		}
		if(type == sf::Event::MouseButtonPressed && (fadeState == Fade_Done && winFocused))
		{
			if (e.mouseButton.button == sf::Mouse::Left)
			{
				guip_eof_LMB_FF = guip_eof;
			}
			if (e.mouseButton.button == sf::Mouse::Right)
			{
				guip_eof_RMB_FF = guip_eof;
			}
			if (e.mouseButton.button == sf::Mouse::Middle)
			{
				guip_eof_MMB_FF = guip_eof;
			}
		}
		if (type == sf::Event::MouseButtonReleased && (fadeState == Fade_Done && winFocused))
		{
			if (e.mouseButton.button == sf::Mouse::Left)
			{
				if(guip_eof_LMB_FF == guip_eof)
				{
					Mouse::LeftRel = true;
				}
				guip_eof_LMB_FF = nullptr;
			}
			if (e.mouseButton.button == sf::Mouse::Right)
			{
				if(guip_eof_RMB_FF == guip_eof)
				{
					Mouse::RightRel = true;
				}
				guip_eof_RMB_FF = nullptr;
			}
			if (e.mouseButton.button == sf::Mouse::Middle)
			{
				if(guip_eof_MMB_FF == guip_eof)
				{
					Mouse::MiddleRel = true;
				}
				guip_eof_MMB_FF = nullptr;
			}
		}
		if (type == sf::Event::JoystickButtonPressed || type == sf::Event::JoystickMoved)
		{
			joystickIndex = e.joystickConnect.joystickId;
			inMethod = InputMethod_Controller;
		}
		if (type == sf::Event::MouseMoved)
		{
			inMethod = InputMethod_Keyboard;
		}
		if(type == sf::Event::KeyPressed && winFocused)
		{
			Kb::KeyFFGuiObj[e.key.code] = guip_eof;
		}
		if(type == sf::Event::KeyReleased && winFocused)
		{
			if(Kb::KeyFFGuiObj[e.key.code] == guip_eof/*  || (e.key.code == KB::Escape || (e.key.code >= KB::F1 && e.key.code <= KB::F12)) */)
			{
				Kb::KeyReleased[e.key.code] = 1;
				Kb::KeyFFGuiObj[e.key.code] = nullptr;
				Kb::KeyHeldFrames[e.key.code] = 0;
			}
		}
		if (type == sf::Event::MouseWheelScrolled)
		{
			Mouse::WheelDelta = (int)e.mouseWheelScroll.delta;
#if DEBUG
			if(IGS && IGS->tileEditOn && Kb::KeyHeldFrames[KB::LShift])
			{
				IGS->selectedLayer = Util::Clamp(IGS->selectedLayer + e.mouseWheelScroll.delta, 0, 2);
			}
			else 
#endif
			if (IGS && guip_eof == IGS)
			{
				IGS->camZoom += e.mouseWheelScroll.delta * 2;
				IGS->camZoom = Util::Clamp(IGS->camZoom, 1.0f, 8.0f);
			}
		}
		if (type == sf::Event::TextEntered && winFocused)
		{
			if (e.text.unicode < 128)
			{
				Kb::LastEnteredText += static_cast<char>(e.text.unicode);
			}
		}
	}
	
	for(unsigned int i = 0; i < KB::KeyCount; ++i)
	{
		if (KB::isKeyPressed((KB::Key)i) && winFocused)
		{
			if(Kb::KeyFFGuiObj[i] == guip_eof)
			{
				++Kb::KeyHeldFrames[i];
				inMethod = InputMethod_Keyboard;
			}
			else {
				Kb::KeyHeldFrames[i] = 0;
				Kb::KeyFFGuiObj[i] = nullptr;
			}
		}
	}
}

void Game::doLoop() 
{
	while(win->isOpen())
	{
		win->resetGLStates();

		Sfx::Engine.update();
		Mouse::Moved = false;
		Mouse::LeftRel = false;
		Mouse::RightRel = false;
		Mouse::MiddleRel = false;
		Mouse::WheelDelta = 0;
		auto sfmp = sf::Mouse::getPosition(*win);
		auto sfws = win->getSize();
		Mouse::Pos_X = Util::ScaleClamped(sfmp.x, 0, sfws.x, 0, Game::ScreenWidth);
		Mouse::Pos_Y = Util::ScaleClamped(sfmp.y, 0, sfws.y, 0, Game::ScreenHeight);
		
		doEvents();
		
		if (IGS && guip_eof == IGS && inMethod == InputMethod_Controller)
		{
			// Right stick vertical zoom
			if (std::abs(Controller::rsY) > 20.0f)
			{
				float zoomStep = (Controller::rsY / 100.0f) * frameDeltaMillis * 0.005f;
				IGS->camZoom -= zoomStep;
				IGS->camZoom = Util::Clamp(IGS->camZoom, 1.0f, 8.0f);
			}
		}

		if (winResized)
		{
			view.reset(sf::FloatRect(0, 0, win->getSize().x, win->getSize().y));
			win->setView(view);
		}

		if (fadeState == Fade_Done && winFocused)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)/*  && guip_eof_LMB_FF == guip_eof */)//buggy so disabled for now
			{
				++Mouse::LeftFrames;
			}
			else
			{
				Mouse::LeftFrames = 0;
				guip_eof_LMB_FF = nullptr;
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)/*  && guip_eof_RMB_FF == guip_eof */)
			{
				++Mouse::RightFrames;
			}
			else
			{
				Mouse::RightFrames = 0;
				guip_eof_RMB_FF = nullptr;
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)/*  && guip_eof_MMB_FF == guip_eof */)
			{
				++Mouse::MiddleFrames;
			}
			else
			{
				Mouse::MiddleFrames = 0;
				guip_eof_MMB_FF = nullptr;
			}
		}
		else
		{
			Mouse::LeftFrames = 0;
			Mouse::RightFrames = 0;
			Mouse::MiddleFrames = 0;
		}
#if DEBUG
		if(fadeState == Fade_Done)
		{
			if (Kb::KeyReleased[KB::F7])
			{
				missionSelectScreen->switchTo();
			}
			if(Kb::KeyReleased[KB::F8])
			{
				CharactorCreatorScreen::Instance->switchTo();
			}
		}
#endif

		//win->clear(sf::Color::Black);

		gameWinRendTex->clear(sf::Color(0, 0, 0, 28));


		if (Screen::CurrentScreen)
		{
			Screen::CurrentScreen->doTick(gameWinRendTex);
		}

		drawFades();

		gameWinRendTex->display();

		sf::Sprite gameWinRendTex_sp;
		gameWinRendTex_sp.setOrigin(sf::Vector2f{(float)ScreenWidth / 2, (float)ScreenHeight / 2});
		gameWinRendTex_sp.setTexture(gameWinRendTex->getTexture(), true);
		gameWinRendTex_sp.setPosition(sf::Vector2f{(float)win->getSize().x / 2, (float)win->getSize().y / 2});
		gameWinRendTex_sp.setScale(sf::Vector2f{(float)win->getSize().x / (float)ScreenWidth, (float)win->getSize().y / (float)ScreenHeight});
		win->draw(gameWinRendTex_sp);

		win->display();

		//scheduled events
		EventScheduler::GlobalScheduler->update(frameDeltaMillis);

		auto gpeofBef = guip_eof;
		guip_eof = guip;
		
		// if (Mouse::LeftFrames == 1)
		// {
		// 	guip_eof_LMB_FF = guip_eof;
		// }

		++framesPassed;
		if (fadeState == Fade_Done)
		{
			++curScreenFramesPassed;
		}

		float curFpsTime = fpsClock.getElapsedSeconds();

		if (fpsClock.getElapsedSeconds() >= 1.0f)
		{
			framesPerSecond = 1.0f / (curFpsTime - lastTimeFps);
			fpsClock.restart();
		}
		lastTimeFps = curFpsTime;

		frameDeltaMillis = (float)frameDeltaClock.getElapsedMicroseconds() / 1000.0f;
		frameDeltaClock.restart();

		if (GameSettings::GlobalSettings->uncapFramerate)
		{			
			win->setFramerateLimit(0);
		}
		else if (!winFocused) 
		{
			win->setFramerateLimit(15);
		}
		else
		{
			win->setFramerateLimit(fpsLimit);
		}

		auto khMillis = (float)KeysHeldClock.getElapsedMilliseconds();

		if(winFocused)
		{
			for (unsigned int i = 0; i < KB::KeyCount; ++i)
			{
				if (Kb::KeyHeldFrames[i])
				{
					Kb::KeysHeldMillis[i] += khMillis;
				}
				else
				{
					Kb::KeysHeldMillis[i] = 0.0f;
				}
			}

			for (unsigned int i = 0; i < sf::Joystick::ButtonCount; ++i)
			{
				if (sf::Joystick::isButtonPressed(0, i))
				{
					Controller::HeldMillis[(Btn)i] += khMillis;
					Controller::BtnFrames[(Btn)i]++;
				}
				else
				{
					if (Controller::BtnRel[(Btn)i])
					{
						Controller::HeldMillis[(Btn)i] = 0.0f;
					}
					Controller::BtnRel[(Btn)i] = false;
					if (Controller::BtnFrames[(Btn)i])
					{
						Controller::BtnRel[(Btn)i] = true;
						//Util::PrintLnFormat("btn rel");
					}
					Controller::BtnFrames[(Btn)i] = 0;
				}
			}
		}

		if(gpeofBef != guip_eof)
		{
			//reset held keys
			for(auto& k : Kb::KeysHeldMillis)
			{
				k.second = 0.0f;
			}
			for(auto& k : Controller::HeldMillis)
			{
				k.second = 0.0f;
			}
			for(auto& k : Controller::BtnFrames)
			{
				k.second = 0;
			}
			for(auto& k : Controller::BtnRel)
			{
				k.second = false;
			}			
			for (unsigned int i = 0; i < KB::KeyCount; ++i)
			{
				Kb::KeyHeldFrames[i] = 0;
				Kb::KeyReleased[i] = 0;
			}
		}

		KeysHeldClock.restart();

		Controller::dpx = sf::Joystick::getAxisPosition(G->joystickIndex, sf::Joystick::Axis::PovX);
		Controller::dpy = sf::Joystick::getAxisPosition(G->joystickIndex, sf::Joystick::Axis::PovY);
		Controller::rsX = sf::Joystick::getAxisPosition(G->joystickIndex, sf::Joystick::Axis::U);
		Controller::rsY = sf::Joystick::getAxisPosition(G->joystickIndex, sf::Joystick::Axis::V);
		Controller::lt = sf::Joystick::getAxisPosition(G->joystickIndex, sf::Joystick::Axis::Z);
		if(Controller::dpx != 0)
		{
			Controller::dpxFrames++;
		}
		else 
		{
			Controller::dpxFrames = 0;
		}
		if(Controller::dpy != 0)
		{
			Controller::dpyFrames++;
		}
		else 
		{
			Controller::dpyFrames = 0;
		}

		winResized = false;
	}
}

void Game::close()
{
	ExitScreen::Instance->switchTo();
}

void Game::drawFades()
{
	if (framesPassed == 0)
	{
		initialFadeInClock.restart();
	}
	float screenSwitchFadeMs = screenSwitchClock.getElapsedMilliseconds();

	float fadeMs = 230;
	
	// TV Static transition logic
	if (screenSwitchFadeMs <= fadeMs)
	{
		// Calculate transition progress (0.0 to 1.0)
		float progress = screenSwitchFadeMs / fadeMs;
		
		// Update TV static shader parameters
		tvStaticShader->setTransitionProgress(progress);
		tvStaticShader->setTime(screenSwitchFadeMs * 0.001f);
		tvStaticShader->setResolution(ScreenWidth, ScreenHeight);
		tvStaticShader->update();
		
		// Create a render texture to capture the current screen
		static sf::RenderTexture renderTexture;
		static bool textureInitialized = false;
		
		if (!textureInitialized || renderTexture.getSize().x != (unsigned int)ScreenWidth || renderTexture.getSize().y != (unsigned int)ScreenHeight)
		{
			renderTexture.create(ScreenWidth, ScreenHeight);
			renderTexture.clear();
			renderTexture.display();
			textureInitialized = true;
		}
		
		// Apply TV static shader to the captured texture
		sf::Sprite sprite(renderTexture.getTexture());
		sprite.setPosition(0, 0);
		
		gameWinRendTex->draw(sprite, &tvStaticShader->shader);
	}
	else
	{
		if (fadeState == Fade_Out)
		{
			fadeState = Fade_In;
			screenSwitchClock.restart();
			if (Screen::CurrentScreen->onLeave() && Screen::ToScreen->onEnter())
			{
				Screen::LastScreen = Screen::CurrentScreen;
				Screen::CurrentScreen = Screen::ToScreen;
				curScreenFramesPassed = 0;
			}
		}
		else if (fadeState == Fade_In)
		{
			fadeState = Fade_Done;
		}
	}

	// Original white fade-in effect (for game start)
	float fadeInTime = 600;
	float fadeInMs = Easing::Apply(Ease_In, initialFadeInClock.getElapsedMilliseconds(), 0, fadeInTime, fadeInTime);
	if (fadeInMs <= fadeInTime)
	{
		auto col = sf::Color(255, 255, 255, Util::Scale(fadeInMs, 0, fadeInTime, 255, 0));
		sf::VertexArray va(sf::PrimitiveType::Quads);
		va.append(sf::Vertex(sf::Vector2f(0, 0), col));
		va.append(sf::Vertex(sf::Vector2f(ScreenWidth, 0), col));
		va.append(sf::Vertex(sf::Vector2f(ScreenWidth, ScreenHeight), col));
		va.append(sf::Vertex(sf::Vector2f(0, ScreenHeight), col));
		gameWinRendTex->draw(va);
	}
}

}