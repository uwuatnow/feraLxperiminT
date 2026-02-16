#pragma once

namespace nyaa {

using BodyStackPart = int;

class Skeleton
{
public:
	
	friend class ProceduralSpriteEditorGUI;

	enum : int
	{
		Flags_0Male_1Female = 1 << 0,
	};
	using Flags = int;
private: //data
	Flags flags;
	float beingHeight;

	class SkeletonJoint* hipTailBoneRootJoint;

	//keep at bottom of data
	class Serializable* serial;

private:
	static float GetBodyStackPartHeight(BodyStackPart part);

public:
	Skeleton(const char* name);

	virtual ~Skeleton();
};

class SkeletonJoint
{
public:
	SkeletonJoint();

	virtual ~SkeletonJoint();
};

class ProceduralSprite
{
public:

	friend class ProceduralSpriteEditorGUI;

	ProceduralSprite(unsigned int widthPix, unsigned int heightPix);

	virtual ~ProceduralSprite();

	void Generate() const;
private:
	unsigned int widthPix, heightPix;
	unsigned int fboHandle, tex, depthTex;

	float rotationX, rotationY, rotationZ;
	float zoom;

	class Skeleton* skeleton;
};

}