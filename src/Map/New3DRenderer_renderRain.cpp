#include "Map/New3DRenderer.h"
#include "Screen/InGameScreen.h"
#include "Game/Game.h"
#include "Texture/Sheet.h"
#include <SFML/OpenGL.hpp>

namespace nyaa {

void New3DRenderer::renderRain3D()
{
    if (!IGS->raining || IGS->RainDrops.empty()) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    
    sf::Texture::bind(&IGS->RainDropsSheet->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Get current modelview matrix to extract camera vectors
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    
    // Extract right and up vectors from camera matrix
    float rightX = modelview[0], rightY = modelview[4], rightZ = modelview[8];
    float /* upX = modelview[1], */upY = modelview[5], upZ = modelview[9];
    
    for (const auto& drop : IGS->RainDrops) {
        // Position rain in camera space for screen-relative positioning
        float screenX = (drop.x / IGS->RainDrops.size()) * 20.0f - 10.0f;
        float screenY = 5.0f - (drop.y / 100.0f) * 10.0f; // Falls downward
        float screenZ = -2.0f; // In front of camera
        
        // Get texture coordinates
        auto coords = sf::Vector2f{};
        IGS->RainDropsSheet->getTexCoordsFromId((int)drop.frame, &coords.x, &coords.y);
        float u1 = coords.x / IGS->RainDropsSheet->tex.getSize().x;
        float v1 = coords.y / IGS->RainDropsSheet->tex.getSize().y;
        float u2 = (coords.x + IGS->RainDropsSheet->tileSizePixelsX) / IGS->RainDropsSheet->tex.getSize().x;
        float v2 = (coords.y + IGS->RainDropsSheet->tileSizePixelsY) / IGS->RainDropsSheet->tex.getSize().y;
        
        // Rain color with transparency
        glColor4f(0.2f, 0.2f, 1.0f, 1.f);
        
        float width = 0.05f;
        float height = 0.6f;
        
        // Create billboard quad using camera vectors
        float x1 = screenX - rightX * width;
        float y1 = screenY - rightY * width - upY * height;
        float z1 = screenZ - rightZ * width - upZ * height;
        
        float x2 = screenX + rightX * width;
        float y2 = screenY + rightY * width - upY * height;
        float z2 = screenZ + rightZ * width - upZ * height;
        
        float x3 = screenX + rightX * width;
        float y3 = screenY + rightY * width + upY * height;
        float z3 = screenZ + rightZ * width + upZ * height;
        
        float x4 = screenX - rightX * width;
        float y4 = screenY - rightY * width + upY * height;
        float z4 = screenZ - rightZ * width + upZ * height;
        
        glBegin(GL_QUADS);
        glTexCoord2f(u1, v2); glVertex3f(x1, y1, z1);
        glTexCoord2f(u2, v2); glVertex3f(x2, y2, z2);
        glTexCoord2f(u2, v1); glVertex3f(x3, y3, z3);
        glTexCoord2f(u1, v1); glVertex3f(x4, y4, z4);
        glEnd();
    }
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}