#pragma once
#include "Game/Globals.h"
#include <string>
#include <vector>

struct ma_engine;
struct ma_sound;

namespace nyaa {

class SoundFxEngine
{
public:
	class Fx
	{
	public:
		Fx(SoundFxEngine *host, std::string name);

		~Fx();

	public:
		void play(bool noSpam = false, float volume = 1.0f); //at player pos

		void play(float x, float y, bool noSpam = false, float volume = 1.0f); //at world pos x,y

		void play(double x, double y, bool noSpam = false, float volume = 1.0f); //at world pos x,y

		void stop();

		bool isPlaying();

	public:
		SoundFxEngine* host;
		std::string name;
		bool randPitch;
		std::vector<ma_sound*> instances;
		static const int maxVoices = 5;
	};

public:
	SoundFxEngine();
	
	~SoundFxEngine();
	
	Fx* add(std::string name, bool randPitch = true);

	void update();

	void setListener(double x, double y, double z, float lookX, float lookY, float lookZ, float upX, float upY, float upZ);

	void play(Fx* fx);

public:
	static bool bypassFLC;

public:
	ma_engine* engine;
	void* flcNode;
	std::vector<Fx*> sounds;
	float masterVolume;
	float radioVolume;
	double listenerX, listenerY, listenerZ;
	double maxHearDistance;
};

extern float flc_basestep;
extern int flc_recon_steps;

}