#include "Texture/Sheet.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include <cassert>

namespace nyaa {

static const int SHEET_PADDING = 2;

Sheet::Sheet(std::string loc, unsigned int sizeX, unsigned int sizeY)
	:sizeX(sizeX)
	,sizeY(sizeY)
	,loc(std::string(Game::ResDir) + loc + ".png")
{
	if (!loc.empty() && Util::FileExists(this->loc)) {
		loadAndPad(this->loc);
	} else {
	    // Fallback if file not found (though loadAndPad handles loading, we need to init logic)
	    auto ts = tex.getSize(); // Likely 0
	    tileSizePixelsX = sizeX > 0 ? ts.x / sizeX : 0;
	    tileSizePixelsY = sizeY > 0 ? ts.y / sizeY : 0;
	}
#if DEBUG
	if (!loc.empty()) G->sheets.push_back(this);
#endif
}

Sheet::~Sheet()
{
#if DEBUG
	/*assert*/(Util::RemoveFromVec(G->sheets, this));
#endif
}

void Sheet::getTexCoordsFromId(unsigned int tileId, float* outX, float* outY)
{
	// Padding on each side (2 * SHEET_PADDING total spacing)
	if (outX) *outX = (tileId % sizeX) * (tileSizePixelsX + 2 * SHEET_PADDING) + SHEET_PADDING;
	if (outY) *outY = (tileId / sizeY) * (tileSizePixelsY + 2 * SHEET_PADDING) + SHEET_PADDING;
}

unsigned int Sheet::getId(unsigned int posX, unsigned int posY)
{
	return (posY * sizeX) + posX;
}

#if DEBUG
void Sheet::reload()
{
	loadAndPad(loc);
}
#endif

void Sheet::loadAndPad(const std::string& path)
{
	sf::Image src;
	if (!src.loadFromFile(path)) return;

	auto ts = src.getSize();
	tileSizePixelsX = sizeX > 0 ? ts.x / sizeX : 0;
	tileSizePixelsY = sizeY > 0 ? ts.y / sizeY : 0;

	if (tileSizePixelsX == 0 || tileSizePixelsY == 0) return;

	int pad = SHEET_PADDING;
	unsigned int newW = sizeX * (tileSizePixelsX + 2 * pad);
	unsigned int newH = sizeY * (tileSizePixelsY + 2 * pad);

	sf::Image dest;
	dest.create(newW, newH, sf::Color::Transparent);

	for (unsigned int y = 0; y < sizeY; ++y) {
		for (unsigned int x = 0; x < sizeX; ++x) {
			int srcX = x * tileSizePixelsX;
			int srcY = y * tileSizePixelsY;
			int dstX = x * (tileSizePixelsX + 2 * pad) + pad;
			int dstY = y * (tileSizePixelsY + 2 * pad) + pad;

			// Copy content
			dest.copy(src, dstX, dstY, sf::IntRect(srcX, srcY, tileSizePixelsX, tileSizePixelsY));

			// Extrude edges (Clamp to edge behavior)
			
			// Top
			dest.copy(src, dstX, dstY - 1, sf::IntRect(srcX, srcY, tileSizePixelsX, 1));
			// Bottom
			dest.copy(src, dstX, dstY + tileSizePixelsY, sf::IntRect(srcX, srcY + tileSizePixelsY - 1, tileSizePixelsX, 1));
			// Left
			dest.copy(src, dstX - 1, dstY, sf::IntRect(srcX, srcY, 1, tileSizePixelsY));
			// Right
			dest.copy(src, dstX + tileSizePixelsX, dstY, sf::IntRect(srcX + tileSizePixelsX - 1, srcY, 1, tileSizePixelsY));
			
			// Corners
			dest.setPixel(dstX - 1, dstY - 1, src.getPixel(srcX, srcY)); // TL
			dest.setPixel(dstX + tileSizePixelsX, dstY - 1, src.getPixel(srcX + tileSizePixelsX - 1, srcY)); // TR
			dest.setPixel(dstX - 1, dstY + tileSizePixelsY, src.getPixel(srcX, srcY + tileSizePixelsY - 1)); // BL
			dest.setPixel(dstX + tileSizePixelsX, dstY + tileSizePixelsY, src.getPixel(srcX + tileSizePixelsX - 1, srcY + tileSizePixelsY - 1)); // BR
		}
	}

	tex.loadFromImage(dest);
	tex.generateMipmap();
}

}
