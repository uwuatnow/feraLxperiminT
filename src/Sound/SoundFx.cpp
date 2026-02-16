#include "Sound/SoundFxEngine.h"
#include <miniaudio.h>
#include "Game/Util.h"
#include <iostream>

namespace nyaa {

SoundFxEngine::Fx::Fx(SoundFxEngine* host, std::string name)
	:host(host)
	,name(name)
	,randPitch(false)
{
	auto wl = Util::GetWAVLoc(name);
	const char* loc = wl.c_str();
	instances.reserve(maxVoices);
	for (int i = 0; i < maxVoices; ++i) {
		ma_sound* s = new ma_sound;
		ma_result result = ma_sound_init_from_file(host->engine, loc, 0, NULL, NULL, s);
		if (result != MA_SUCCESS) {
			std::cout << Util::Format("Failed to load sfx %s\n", loc);
		} else {
            // Attach to FLC node
            ma_node_attach_output_bus((ma_node*)s, 0, (ma_node*)host->flcNode, 0);
        }
		instances.push_back(s);
	}
}

SoundFxEngine::Fx::~Fx()
{
	for (auto s : instances) {
		ma_sound_uninit(s);
		delete s;
	}
	instances.clear();
}

void SoundFxEngine::Fx::play(bool noSpam, float volume)
{
	for (auto s : instances) {
		if (!ma_sound_is_playing(s)) {
			if (randPitch) {
				float rand = Util::RandNormalized() / 2;
				ma_sound_set_pitch(s, 1.0f + rand);
			}
			ma_sound_seek_to_pcm_frame(s, 0);
			ma_sound_set_positioning(s, ma_positioning_relative);
			ma_sound_set_position(s, 0, 0, 0);
			ma_sound_set_volume(s, volume);
			ma_sound_start(s);
			return;
		}
	}
	// All voices busy, do nothing
}

void SoundFxEngine::Fx::play(float x, float y, bool noSpam, float volume)
{
	play((double)x, (double)y, noSpam, volume);
}

void SoundFxEngine::Fx::play(double x, double y, bool noSpam, float volume)
{
	// Calculate distance to listener in double precision
	double dx = x - host->listenerX;
	double dy = y - host->listenerZ; // wait, listenerZ is used for second world coord? check setListener
	double distSq = dx * dx + dy * dy;
	if (distSq > host->maxHearDistance * host->maxHearDistance) return;

	for (auto s : instances) {
		if (!ma_sound_is_playing(s)) {
			if (randPitch) {
				float rand = Util::RandNormalized() / 2.0f;
				ma_sound_set_pitch(s, 1.0f + rand);
			}
			ma_sound_seek_to_pcm_frame(s, 0);
			ma_sound_set_positioning(s, ma_positioning_absolute);
			ma_sound_set_position(s, (float)x, 0.0f, (float)y);
			ma_sound_set_min_distance(s, 16.0f * 3.0f); // 3 tiles
			ma_sound_set_max_distance(s, (float)host->maxHearDistance);
			ma_sound_set_volume(s, volume);
			ma_sound_start(s);
			return;
		}
	}
	// All voices busy, do nothing
}

void SoundFxEngine::Fx::stop()
{
	for (auto s : instances) {
		ma_sound_stop(s);
	}
}

bool SoundFxEngine::Fx::isPlaying()
{
	for (auto s : instances) {
		if (ma_sound_is_playing(s)) {
			return true;
		}
	}
	return false;
}

}