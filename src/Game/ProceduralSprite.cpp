#include "Game/ProceduralSprite.h"
#include <cassert>
#include <math.h>
#include "Serialize/Serializable.h"
#include "Game/Util.h"
#include "Game/BodyStackPart.h"

namespace nyaa {

const float BodyStackPartHeightTable[] =
{
#define X(name, height) height,
	NYAA_BODY_STACK_PARTS
#undef X
};

float Skeleton::GetBodyStackPartHeight(BodyStackPart part)
{
	if(part >= 0 && part < BodyStackPart_Count)
	{
		return BodyStackPartHeightTable[part];
	}

	return 0.0f;
}

Skeleton::Skeleton(const char* name)
	:flags(0)
	,beingHeight(2.0f)
	,hipTailBoneRootJoint(nullptr)
	,serial(new Serializable("skeleton", nullptr, Util::Format("%s.json", name)))
{
	NYAA_SERIAL_ADD(serial, flags,);
	NYAA_SERIAL_ADD(serial, beingHeight,);

	serial->Read();
}

Skeleton::~Skeleton()
{
	serial->upload();
	delete serial; //keep at bottom
}

ProceduralSprite::ProceduralSprite(unsigned int widthPix, unsigned int heightPix)
	:widthPix(widthPix)
	,heightPix(heightPix)
	,fboHandle(0)
	,tex(0)
	,depthTex(0)
	,rotationX(0.0f)
	,rotationY(0.0f)
	,rotationZ(0.0f)
	,zoom(1.0f)
	,skeleton(new Skeleton("test_skeleton"))
{
	// fboHandle = rlLoadFramebuffer();
	
	assert(widthPix <= 640);
	assert(heightPix <= 480);

	// tex = rlLoadTexture(nullptr, widthPix, heightPix, 
	// 	RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
	// rlFramebufferAttach(fboHandle, tex, 
	// 	RL_ATTACHMENT_COLOR_CHANNEL0, 
	// 	RL_ATTACHMENT_TEXTURE2D, 
	// 	0
	// );

	// depthTex = rlLoadTextureDepth(widthPix, heightPix, true);
	// rlFramebufferAttach(fboHandle, depthTex, 
	// 	RL_ATTACHMENT_DEPTH, 
	// 	RL_ATTACHMENT_RENDERBUFFER, 
	// 	0
	// );
	
	// rlFramebufferComplete(fboHandle);
}

ProceduralSprite::~ProceduralSprite()
{
	delete skeleton;
}

void ProceduralSprite::Generate() const
{
	//rotation++;

	// rlEnableFramebuffer(fboHandle);
	// rlViewport(0, 0, widthPix, heightPix);
	// rlClearColor(0, 0, 0, 255);
	// rlClearScreenBuffers();

	// rlMatrixMode(RL_PROJECTION);
	// rlLoadIdentity();
	// rlFrustum(-1, 1, -1, 1, 1, 20);

	// rlMatrixMode(RL_MODELVIEW);
	// rlLoadIdentity();
	// rlTranslatef(0, 0, -3 + (zoom - 1.0f));
	// rlRotatef(fmodf(rotationY, 360), 0,1,0);
	// rlRotatef(fmodf(rotationX, 360), 1,0,0);
	// rlRotatef(fmodf(rotationZ, 360), 0,0,1);

	// rlBegin(RL_TRIANGLES);
	// rlColor3f(1, 1, 1);
	// rlVertex3f(0, 1, 0);
	// rlVertex3f(-1, -1, 0);
	// rlVertex3f(1, -1, 0);
	// rlEnd();

	// rlDrawRenderBatchActive();

	// rlDisableFramebuffer();
}

SkeletonJoint::SkeletonJoint()
{
}

SkeletonJoint::~SkeletonJoint()
{
}

}
