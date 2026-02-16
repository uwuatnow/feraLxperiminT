#pragma once

namespace nyaa {

enum TileFlags : char {
	TileFlags_None,
	TileFlags_CantWalkHere,
	TileFlags_Stonelike,
	TileFlags_Grasslike,
	TileFlags_Dirtlike,
	TileFlags_Rotate90,
	TileFlags_Rotate180,
	TileFlags_Rotate270
};

}