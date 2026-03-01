#include "Sound/SoundFxEngine.h"
#include <miniaudio.h>
#include <iostream>
#include <cassert>
#include "Game/Util.h"
#include "Thonk/FLC.h"

#define M_PI 3.14159265358979323846f

namespace nyaa {

bool SoundFxEngine::bypassFLC = false;

static flc::FLC flc_left;
static flc::FLC flc_right;
static std::vector<float> left_buffer;
static std::vector<float> right_buffer;
static float flc_last_left = 0.0f;
static float flc_last_right = 0.0f;
float flc_basestep = 0.26f;
int flc_recon_steps = 12;

static void flc_node_process(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    const float* pIn = ppFramesIn[0];
    float* pOut = ppFramesOut[0];
    ma_uint32 frameCount = *pFrameCountOut;
    int channels = ma_node_get_output_channels(pNode, 0);
    float basestep = flc_basestep > 0.0f ? flc_basestep : 0.000001f;
    if (SoundFxEngine::bypassFLC)
    {
        //std::cout << "FLC bypassed\n";
        for (ma_uint32 i = 0; i < frameCount; ++i) {
            for (int ch = 0; ch < channels; ++ch) {
                pOut[i * channels + ch] = pIn[i * channels + ch];
            }
        }
        
        // Track last values even when bypassed so engaging isn't harsh
        if (frameCount > 0) {
            ma_uint32 lastIdx = frameCount - 1;
            if (channels >= 1) flc_last_left = pOut[lastIdx * channels + 0];
            if (channels >= 2) flc_last_right = pOut[lastIdx * channels + 1];
        }
        return;
    }


    left_buffer.resize(frameCount);
    right_buffer.resize(frameCount);

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        if (channels >= 1) left_buffer[i] = pIn[i * channels + 0];
        if (channels >= 2) right_buffer[i] = pIn[i * channels + 1];
    }

    int fadeBytes = 255;
    if (channels >= 1) {
        auto [q, bands] = flc_left.flc_compress(left_buffer, basestep);
        auto recon = flc_recon_steps != 12 ? flc_left.progressive_recon(flc_recon_steps, q, bands, basestep) : flc_left.flc_decompress(q, bands, basestep);

        if (!recon.empty()) {
            float diff = flc_last_left - recon[0];
            if (fadeBytes > (int)recon.size()) fadeBytes = (int)recon.size();
            for (int k = 0; k < fadeBytes; k++) {
                recon[k] += diff * (1.0f - ((float)k / (float)fadeBytes));
            }
            flc_last_left = recon.back();
        }

        for (ma_uint32 i = 0; i < frameCount; ++i) {
            if (i < recon.size()) {
                float val = recon[i];
                pOut[i * channels + 0] = val;
            }
        }
    }

    if (channels >= 2) {
        auto [q, bands] = flc_right.flc_compress(right_buffer, basestep);
        auto recon = flc_recon_steps != 12 ? flc_right.progressive_recon(flc_recon_steps, q, bands, basestep) : flc_right.flc_decompress(q, bands, basestep);

        if (!recon.empty()) {
            float diff = flc_last_right - recon[0];
            if (fadeBytes > (int)recon.size()) fadeBytes = (int)recon.size();
            for (int k = 0; k < fadeBytes; k++) {
                recon[k] += diff * (1.0f - ((float)k / (float)fadeBytes));
            }
            flc_last_right = recon.back();
        }

        for (ma_uint32 i = 0; i < frameCount; ++i) {
             if (i < recon.size()) {
                float val = recon[i];
                pOut[i * channels + 1] = val;
             }
        }
    }
}

static ma_node_vtable flc_node_vtable = {
    flc_node_process,
    NULL,
    1,
    1,
    MA_NODE_FLAG_CONTINUOUS_PROCESSING
};

SoundFxEngine::SoundFxEngine()
:masterVolume(
#if DEBUG
0.2f
#else
0.91f
#endif
)
,radioVolume(0.6f)
, listenerX(0.0), listenerY(0.0), listenerZ(0.0)
, maxHearDistance(16.0 * 25.0)
{
	engine = new ma_engine;
	ma_engine_config config = ma_engine_config_init();
	config.periodSizeInFrames = 256;
	ma_result result = ma_engine_init(&config, engine);
	if (result != MA_SUCCESS) {
		std::cout << "Failed to initialize miniaudio engine\n";
	}
    flc_last_left = 0.0f;
    flc_last_right = 0.0f;
	ma_engine_set_volume(engine, masterVolume);

    // Initialize FLC Node
    flcNode = new ma_node_base;
    ma_node_config nodeConfig = ma_node_config_init();
    nodeConfig.vtable = &flc_node_vtable;
    static ma_uint32 channels[1] = {2}; // Assume stereo
    nodeConfig.pInputChannels = channels;
    nodeConfig.pOutputChannels = channels;
    
    ma_node_init(ma_engine_get_node_graph(engine), &nodeConfig, NULL, (ma_node*)flcNode);
    ma_node_attach_output_bus((ma_node*)flcNode, 0, ma_engine_get_endpoint(engine), 0);
}

SoundFxEngine::~SoundFxEngine()
{
	for(auto s : sounds) delete s;
	sounds.clear();

    if (flcNode) {
        ma_node_uninit((ma_node_base*)flcNode, NULL);
        delete (ma_node_base*)flcNode;
    }

	ma_engine_uninit(engine);
	delete engine;
}

SoundFxEngine::Fx* SoundFxEngine::add(std::string name, bool randPitch) 
{
	for (auto s : sounds)
	{
		if (s->name == name)
		{
			return s;
		}
	}
	auto sfx = new Fx(this, name);
	sounds.push_back(sfx);
	sfx->randPitch = randPitch;
	return sfx;
}

void SoundFxEngine::update()
{
	//Util::PrintLnFormat("SoundFxEngine::update() masterVolume: %f", masterVolume);
	ma_engine_set_volume(engine, masterVolume);
}

void SoundFxEngine::setListener(double x, double y, double z, float lookX, float lookY, float lookZ, float upX, float upY, float upZ)
{
	listenerX = x;
	listenerY = y;
	listenerZ = z;
	ma_engine_listener_set_position(engine, 0, (float)x, (float)y, (float)z);
	ma_engine_listener_set_direction(engine, 0, lookX, lookY, lookZ);
	ma_engine_listener_set_world_up(engine, 0, upX, upY, upZ);
}

void SoundFxEngine::play(Fx* fx)
{
	fx->play();
}

}