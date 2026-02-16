#include "Map/New3DRenderer.h"
#include "Shader/New3DMainShader.h"
#include "Model/ObjLoader.h"
#include "Map/Map.h"
#include "Map/MapChunk.h"
#include "Map/Pathfinding.h"
#include "Texture/Sheet.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Entity/Light.h"
#include "Entity/Laptop.h"
#include "Entity/Prop.h"
#include "Entity/Computer.h"
#include "Entity/BulletProjectile.h"
#include "Entity/DevCar.h"
#include "Entity/Door.h"
#include "Entity/BuildingWallEntity.h"
#include "Texture/WalkAnim.h"
#include "Inventory/Gun.h"
#include "Inventory/Item.h"
#include "Inventory/GasCan.h"
#include "Game/Util.h"
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include <cmath>
#include <algorithm>
#include "Game/Game.h"
#include "Inventory/Inventory.h"
#include "Game/Fonts.h"

namespace nyaa {

void New3DRenderer::renderObjects(Map* map, RendTarget& target)
{
    // Collect all renderable objects (obstacles and entities) for depth sorting
    struct RenderObject {
        enum Type { OBSTACLE, ENTITY } type;
        double sortY; // Y coordinate for sorting
        
        // For obstacles (relative to camera origin)
        float relX, relZ;
        unsigned int tileId;
        
        // For entities
        Entity* entity;
        
        RenderObject(Type t, double y) : type(t), sortY(y), entity(nullptr) {}
    };
    
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;
    
    std::vector<RenderObject> renderObjects;
    
    // Collect obstacle tiles from all chunks
    for (auto c : map->chunks) {
        double chunkX = (double)c->x * (double)MapChunk::SizeTiles * (double)tileSize;
        double chunkZ = (double)c->y * (double)MapChunk::SizeTiles * (double)tileSize;
        
        for (unsigned int y = 0; y < MapChunk::SizeTiles; ++y) {
            for (unsigned int x = 0; x < MapChunk::SizeTiles; ++x) {
                auto& obstacleTile = c->getTILE(x, y, 1);
                if (obstacleTile.id != 0) {
                    double worldZ = chunkZ + (double)y * (double)tileSize;
                    double pixelY = (worldZ + (double)tileSize) * 16.0;
                    RenderObject obj(RenderObject::OBSTACLE, pixelY);
                    obj.relX = (float)(chunkX + (double)x * (double)tileSize - camWorldX);
                    obj.relZ = (float)(worldZ - camWorldZ);
                    obj.tileId = obstacleTile.id;
                    renderObjects.push_back(obj);
                }
            }
        }
    }
    
    // Collect entities
    for (auto& entity : map->entities) {
        RenderObject obj(RenderObject::ENTITY, entity->posY - entity->posZ);
        obj.entity = entity;
        renderObjects.push_back(obj);
    }
    
    // Sort by Y coordinate (lower Y renders first)
    std::sort(renderObjects.begin(), renderObjects.end(), [](const RenderObject& a, const RenderObject& b) {
        return a.sortY < b.sortY;
    });
    
    // Render sorted objects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    for (const auto& obj : renderObjects) {
        if (obj.type == RenderObject::OBSTACLE) {
            glDepthMask(GL_TRUE);
            // Render obstacle tile
            auto coords = sf::Vector2f{};
            map->sheet.getTexCoordsFromId(obj.tileId, &coords.x, &coords.y);
            float u1 = coords.x / map->sheet.tex.getSize().x;
            float v1 = coords.y / map->sheet.tex.getSize().y;
            float u2 = (coords.x + (float)Map::TileSizePixels) / map->sheet.tex.getSize().x;
            float v2 = (coords.y + (float)Map::TileSizePixels) / map->sheet.tex.getSize().y;
            
            sf::Texture::bind(&map->sheet.tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glColor3f(1.0f, 1.0f, 1.0f);
            
            // Set normal facing slightly up and towards camera for vertical wall tiles
            glNormal3f(0.0f, 0.5f, 0.866f);
            
            glBegin(GL_QUADS);
            glTexCoord2f(u1, v2); glVertex3f(obj.relX, 0, obj.relZ + tileSize);
            glTexCoord2f(u2, v2); glVertex3f(obj.relX + tileSize, 0, obj.relZ + tileSize);
            glTexCoord2f(u2, v1); glVertex3f(obj.relX + tileSize, tileSize, obj.relZ + tileSize);
            glTexCoord2f(u1, v1); glVertex3f(obj.relX, tileSize, obj.relZ + tileSize);
            glEnd();
        } else {
            glDepthMask(GL_FALSE);
            // Render entity
            Entity* entity = obj.entity;
            
            // Convert entity position from pixels to world coordinates (relative to camera)
            float entityX = (float)(entity->posX / 16.0 - camWorldX);
            float entityZ = (float)(entity->posY / 16.0 - camWorldZ);

            // Use entity size for quad dimensions, converted to world scale, and apply render scale
            float width = entity->sizeX / 16.0f * entity->renderScaleX;
            float height = entity->sizeY / 16.0f * entity->renderScaleY;

            // Apply origin offset based on normalized origin
            float offsetX = -width * entity->originX;
            float offsetY = 0.0f; // Start at ground level
            
            // Check if this is a Car to render with 3D model
            if (CarBase* car = dynamic_cast<CarBase*>(entity))
            {
                if (!carModelLoaded) {
                    if (ObjLoader::Load(Util::GetResLoc("car.obj"), carModel)) {
                        carModelLoaded = true;
                    } 
                }

                glBindTexture(GL_TEXTURE_2D, whiteTexture);
                glColor3f(1.0f, 1.0f, 1.0f); // White tint for textured model (or magenta if texture missing)
                
                if (carModelLoaded) {
                    glPushMatrix();

                    // Manual Model Matrix Construction for Shader
                    // M = T * S * Ry * Rz
                    float tx = entityX;
                    float ty = 0.05f;
                    float tz = entityZ;
                    float sx = entity->renderScaleX;
                    float sy = entity->renderScaleY;
                    float sz = entity->renderScaleX;
                    
                    float ryRad = Util::ToRad(-car->dirAngle + 90);
                    float rzRad = Util::ToRad(car->bodyRoll);
                    
                    float cy = std::cos(ryRad);
                    float sy_ = std::sin(ryRad);
                    float cz = std::cos(rzRad);
                    float sz_ = std::sin(rzRad);
                    
                    // Col-Major 4x4
                    // R = Ry * Rz
                    // M = S * R -> Scale rows
                    // T -> Add translation to last col
                    
                   /* 
                    Ry (Yaw) * Rz (Roll):
                    [ cy 0 sy ]   [ cz -sz 0 ]
                    [ 0  1 0  ] * [ sz  cz 0 ]
                    [-sy 0 cy ]   [ 0   0  1 ]
                    
                    Row0: cy*cz, -cy*sz, sy
                    Row1: sz,     cz,    0
                    Row2: -sy*cz, sy*sz, cy
                   */

                    float mat[16];
                    // Col 0
                    mat[0]  = sx * (cy * cz);
                    mat[1]  = sy * (sz_);
                    mat[2]  = sz * (-sy_ * cz);
                    mat[3]  = 0.0f;
                    
                    // Col 1
                    mat[4]  = sx * (-cy * sz_);
                    mat[5]  = sy * (cz);
                    mat[6]  = sz * (sy_ * sz_);
                    mat[7]  = 0.0f;
                    
                    // Col 2
                    mat[8]  = sx * (sy_);
                    mat[9]  = 0.0f;
                    mat[10] = sz * (cy);
                    mat[11] = 0.0f;
                    
                    // Col 3
                    mat[12] = tx;
                    mat[13] = ty;
                    mat[14] = tz; 
                    mat[15] = 1.0f;
                    
                    m_new3DMainShader->shader.setUniform("u_modelMatrix", sf::Glsl::Mat4(mat));

                    // Enable depth testing for the 3D model to render correctly
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_BLEND); // Disable blending for opaque car
                    glEnable(GL_CULL_FACE); // Enable culling for proper 3D appearance
                    glCullFace(GL_BACK);

                    carModel.draw();

                    glEnable(GL_BLEND); // Re-enable blending for sprites
                    // glDisable(GL_DEPTH_TEST); // Disable depth test for sprites
                    glDisable(GL_CULL_FACE); 
                    glPopMatrix();

                    // Reset Identity for next objects
                    m_new3DMainShader->shader.setUniform("u_modelMatrix", sf::Glsl::Mat4(sf::Transform::Identity));
                } else {
                     // Fallback if load failed
                    glDepthMask(GL_TRUE);
                    glColor3f(1.0f, 0.0f, 1.0f); // Bright magenta

                    glPushMatrix();
                    glTranslatef(entityX + width/2, 0.5f, entityZ + height/2);
                    glScalef(entity->renderScaleX, entity->renderScaleY, entity->renderScaleX);
                    glRotatef(-car->dirAngle + 90, 0, 1, 0);

                    // Apply body roll
                    glRotatef(car->bodyRoll, 0, 0, 1);
                    
                    // Draw 3D box centered at origin with texture coordinates
                    glBegin(GL_QUADS);
                    // Front
                    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
                    // Back
                    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, -1.0f);
                    // Top
                    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);
                    // Left
                    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
                    // Right
                    glNormal3f(1.0f, 0.0f, 0.0f);
                    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 0.0f, -1.0f);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 1.0f);
                    glEnd();
                    
                    glPopMatrix();
                }
            }
           
            else if (Door* door = dynamic_cast<Door*>(entity))
            {
                glDepthMask(GL_TRUE);
                // Render door as 3D box with thickness
                sf::Texture::bind(nullptr);
                glColor4f(0.6f, 0.4f, 0.2f, 1.0f); // Brown color

                float doorWidth = door->sizeX / 16.f * door->renderScaleX;
                float doorHeight = 2.0f * door->renderScaleY;
                float doorThickness = door->sizeY / 16.f * door->renderScaleX; // assuming thickness scales with X
                float swingAngle = door->dirAngle;
                
                glPushMatrix();
                glTranslatef(entityX, 0.0f, entityZ);
                glRotatef(swingAngle, 0, 1, 0);
                
                // Door box
                glBegin(GL_QUADS);
                // Front face
                glVertex3f(0, 0, 0);
                glVertex3f(doorWidth, 0, 0);
                glVertex3f(doorWidth, doorHeight, 0);
                glVertex3f(0, doorHeight, 0);
                // Back face
                glVertex3f(doorWidth, 0, doorThickness);
                glVertex3f(0, 0, doorThickness);
                glVertex3f(0, doorHeight, doorThickness);
                glVertex3f(doorWidth, doorHeight, doorThickness);
                // Top face
                glVertex3f(0, doorHeight, 0);
                glVertex3f(doorWidth, doorHeight, 0);
                glVertex3f(doorWidth, doorHeight, doorThickness);
                glVertex3f(0, doorHeight, doorThickness);
                // Left face
                glVertex3f(0, 0, doorThickness);
                glVertex3f(0, 0, 0);
                glVertex3f(0, doorHeight, 0);
                glVertex3f(0, doorHeight, doorThickness);
                // Right face
                glNormal3f(std::cos(Util::ToRad(swingAngle)), 0, std::sin(Util::ToRad(swingAngle)));
                glVertex3f(doorWidth, 0, 0);
                glVertex3f(doorWidth, 0, doorThickness);
                glVertex3f(doorWidth, doorHeight, doorThickness);
                glVertex3f(doorWidth, doorHeight, 0);
                glEnd();
                
                // Door knob
                glColor4f(1.0f, 0.8f, 0.0f, 1.0f);
                float knobX = doorWidth - 0.1f;
                float knobSize = 0.05f;
                
                glBegin(GL_QUADS);
                glVertex3f(knobX - knobSize, doorHeight * 0.5f - knobSize, -0.01f);
                glVertex3f(knobX + knobSize, doorHeight * 0.5f - knobSize, -0.01f);
                glVertex3f(knobX + knobSize, doorHeight * 0.5f + knobSize, -0.01f);
                glVertex3f(knobX - knobSize, doorHeight * 0.5f + knobSize, -0.01f);
                glEnd();
                
                glPopMatrix();
            }
            else if (Light* light = dynamic_cast<Light*>(entity))
            {
                glDepthMask(GL_TRUE);
                // Render lamp post
                glBindTexture(GL_TEXTURE_2D, whiteTexture);
                glColor3f(0.3f, 0.3f, 0.3f);

                float postWidth = 0.1f * light->renderScaleX;
                float postHeight = 2.0f * light->renderScaleY;
                
                float relEntityX = (float)(entity->posX / 16.0 - camWorldX);
                float relEntityZ = (float)(entity->posY / 16.0 - camWorldZ);

                glBegin(GL_TRIANGLES);
                // Front face
                glVertex3f(relEntityX - postWidth, 0.0f, relEntityZ - postWidth);
                glVertex3f(relEntityX + postWidth, 0.0f, relEntityZ - postWidth);
                glVertex3f(relEntityX - postWidth, postHeight, relEntityZ - postWidth);
                
                glVertex3f(relEntityX + postWidth, 0.0f, relEntityZ - postWidth);
                glVertex3f(relEntityX + postWidth, postHeight, relEntityZ - postWidth);
                glVertex3f(relEntityX - postWidth, postHeight, relEntityZ - postWidth);
                
                // Back face
                glVertex3f(relEntityX + postWidth, 0.0f, relEntityZ + postWidth);
                glVertex3f(relEntityX - postWidth, 0.0f, relEntityZ + postWidth);
                glVertex3f(relEntityX + postWidth, postHeight, relEntityZ + postWidth);
                
                glVertex3f(relEntityX - postWidth, 0.0f, relEntityZ + postWidth);
                glVertex3f(relEntityX - postWidth, postHeight, relEntityZ + postWidth);
                glVertex3f(relEntityX + postWidth, postHeight, relEntityZ + postWidth);
                
                // Left face
                glVertex3f(entityX - postWidth, 0.0f, entityZ + postWidth);
                glVertex3f(entityX - postWidth, 0.0f, entityZ - postWidth);
                glVertex3f(entityX - postWidth, postHeight, entityZ + postWidth);
                
                glVertex3f(entityX - postWidth, 0.0f, entityZ - postWidth);
                glVertex3f(entityX - postWidth, postHeight, entityZ - postWidth);
                glVertex3f(entityX - postWidth, postHeight, entityZ + postWidth);
                
                // Right face
                glVertex3f(entityX + postWidth, 0.0f, entityZ - postWidth);
                glVertex3f(entityX + postWidth, 0.0f, entityZ + postWidth);
                glVertex3f(entityX + postWidth, postHeight, entityZ - postWidth);
                
                glVertex3f(entityX + postWidth, 0.0f, entityZ + postWidth);
                glVertex3f(entityX + postWidth, postHeight, entityZ + postWidth);
                glVertex3f(entityX + postWidth, postHeight, entityZ - postWidth);
                
                // Lamp bulb
                glColor3f(light->colorR / 255.0f, light->colorG / 255.0f, light->colorB / 255.0f);
                float bulbRadius = 0.2f * light->renderScaleX;
                for (int i = 0; i < 8; ++i) {
                    float angle1 = i * 3.14159f * 2.0f / 8.0f;
                    float angle2 = (i + 1) * 3.14159f * 2.0f / 8.0f;
                    glVertex3f(entityX, postHeight, entityZ);
                    glVertex3f(entityX + cos(angle1) * bulbRadius, postHeight, entityZ + sin(angle1) * bulbRadius);
                    glVertex3f(entityX + cos(angle2) * bulbRadius, postHeight, entityZ + sin(angle2) * bulbRadius);
                }
                glEnd();
            }
            // Check if this is an Actor to render with proper textures
            else if (Actor* actor = dynamic_cast<Actor*>(entity))
            {
                // Adjust offsetY for height
                offsetY = actor->posZ / 16.0f;

                // Render equipped item behind actor if needed
                bool showEquippedItem = actor != IGS->player || actor->isControllable;
                if (actor->inv->equippedItem && (actor->visualDir == Direction_Left || actor->visualDir == Direction_Up) && showEquippedItem)
                {
                    if (Gun* gun = dynamic_cast<Gun*>(actor->inv->equippedItem))
                    {
                        // Update gun angle (same logic as Gun::drawOnActor)
                        if (!IGS->dbox.showing && G->inMethod == InputMethod_Keyboard && actor == IGS->player && !(actor->flags & Entity::EntFlag_Animating) && guip_eof == IGS)
                        {
                            double handX, handY;
                            actor->getHandPos(handX, handY);
                            gun->gunAngle = Util::RotateTowards(handX, handY, IGS->mousePosOnMapX, IGS->mousePosOnMapY);
                            actor->dirAngle = gun->gunAngle;
                        }
                        else
                        {
                            gun->gunAngle = actor->dirAngle;
                        }

                        double handPosX, handPosY;
                        actor->getHandPos(handPosX, handPosY);
                        float handX = (float)(handPosX / 16.0 - camWorldX);
                        float handZ = (float)(handPosY / 16.0 - camWorldZ);
                        float handY = 0.5f + (float)(actor->posZ / 16.0);

                        float recoilOffset = gun->recoilT.secs() * 0.2f;

                        sf::Texture::bind(&gun->tex->tex);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                        glPushMatrix();
                        glTranslatef(handX, handY, handZ);
                        glScalef(actor->renderScaleX, actor->renderScaleY, actor->renderScaleX);
                        glRotatef(-gun->gunAngle + 90, 0, 1, 0);
                        glTranslatef(-recoilOffset, 0, 0);
                        
                        // Gun handle at origin (hand position), barrel extends forward
                        glBegin(GL_QUADS);
                        glTexCoord2f(0.0f, 1.0f); glVertex3f(0, -0.2f, 0);
                        glTexCoord2f(1.0f, 1.0f); glVertex3f(0, -0.2f, 1.2f);
                        glTexCoord2f(1.0f, 0.0f); glVertex3f(0, 0.4f, 1.2f);
                        glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0.4f, 0);
                        glEnd();
                        
                        glPopMatrix();
                    }
                }
                
                // Render actor layers in proper order
                auto renderLayer = [&](WalkAnim& anim, sf::Color color) {
                    if (anim.tex.getSize().x > 0 && anim.columns > 0 && anim.rows > 0)
                    {
                        // Determine row and flip based on direction (matching WalkAnim logic)
                        unsigned int row = 0;
                        bool flip = false;
                        switch (actor->visualDir) {
                        case Direction_Up: row = 0; break;
                        case Direction_Down: row = 1; break;
                        case Direction_Left: row = 2; break;
                        case Direction_Right: row = 2; flip = true; break;
                        default: break;
                        }
                        
                        // Calculate texture coordinates for current frame
                        int frameWidth = anim.tex.getSize().x / anim.columns;
                        int frameHeight = anim.tex.getSize().y / anim.rows;
                        int frameX = anim.frame * frameWidth;
                        int frameY = row * frameHeight;
                        
                        float u1 = (float)frameX / (float)anim.tex.getSize().x;
                        float v1 = (float)frameY / (float)anim.tex.getSize().y;
                        float u2 = (float)(frameX + frameWidth) / (float)anim.tex.getSize().x;
                        float v2 = (float)(frameY + frameHeight) / (float)anim.tex.getSize().y;
                        
                        // Flip texture coordinates horizontally if needed
                        if (flip) {
                            float temp = u1;
                            u1 = u2;
                            u2 = temp;
                        }
                        
                        sf::Texture::bind(&anim.tex);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glColor4f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
                        
                        // Set normal facing slightly up and towards camera for billboard sprites
                        glNormal3f(0.0f, 0.5f, 0.866f);
                        
                        glBegin(GL_QUADS);
                        glTexCoord2f(u1, v2); glVertex3f(entityX + offsetX, offsetY, entityZ);
                        glTexCoord2f(u2, v2); glVertex3f(entityX + offsetX + width, offsetY, entityZ);
                        glTexCoord2f(u2, v1); glVertex3f(entityX + offsetX + width, offsetY + height, entityZ);
                        glTexCoord2f(u1, v1); glVertex3f(entityX + offsetX, offsetY + height, entityZ);
                        glEnd();
                    }
                };
                
                // Render layers in order: body, eyes, hair, clothes, tail, outline
                renderLayer(*actor->body, sf::Color(actor->bodyR, actor->bodyG, actor->bodyB, actor->bodyA));
                renderLayer(*actor->eyes, sf::Color(actor->eyeR, actor->eyeG, actor->eyeB, actor->eyeA));
                renderLayer(*actor->hair, sf::Color(actor->hairR, actor->hairG, actor->hairB, actor->hairA));
                renderLayer(*actor->clothes, sf::Color(actor->clothesR, actor->clothesG, actor->clothesB, actor->clothesA));
                renderLayer(*actor->tail, sf::Color(actor->tailR, actor->tailG, actor->tailB, actor->tailA));
                renderLayer(*actor->outline, sf::Color(actor->outlineR, actor->outlineG, actor->outlineB, actor->outlineA));
                
                // Render equipped item in front of actor if needed
                if (actor->inv->equippedItem && (actor->visualDir == Direction_Right || actor->visualDir == Direction_Down) && showEquippedItem)
                {
                    if (Gun* gun = dynamic_cast<Gun*>(actor->inv->equippedItem))
                    {
                        // Update gun angle (same logic as Gun::drawOnActor)
                        if (!IGS->dbox.showing && G->inMethod == InputMethod_Keyboard && actor == IGS->player && !(actor->flags & Entity::EntFlag_Animating) && guip_eof == IGS)
                        {
                            double handX, handY;
                            actor->getHandPos(handX, handY);
                            gun->gunAngle = Util::RotateTowards(handX, handY, IGS->mousePosOnMapX, IGS->mousePosOnMapY);
                            actor->dirAngle = gun->gunAngle;
                        }
                        else
                        {
                            gun->gunAngle = actor->dirAngle;
                        }

                        double handPosX, handPosY;
                        actor->getHandPos(handPosX, handPosY);
                        float handX = (float)(handPosX / 16.0 - camWorldX);
                        float handZ = (float)(handPosY / 16.0 - camWorldZ);
                        float handY = 0.5f + actor->posZ / 16.0f;

                        float recoilOffset = gun->recoilT.secs() * 0.2f;

                        sf::Texture::bind(&gun->tex->tex);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                        glPushMatrix();
                        glTranslatef(handX, handY, handZ);
                        glScalef(actor->renderScaleX, actor->renderScaleY, actor->renderScaleX);
                        glRotatef(-gun->gunAngle + 90, 0, 1, 0);
                        glTranslatef(-recoilOffset, 0, 0);
                        
                        // Gun handle at origin (hand position), barrel extends forward
                        glBegin(GL_QUADS);
                        glTexCoord2f(0.0f, 1.0f); glVertex3f(0, -0.2f, 0);
                        glTexCoord2f(1.0f, 1.0f); glVertex3f(0, -0.2f, 1.2f);
                        glTexCoord2f(1.0f, 0.0f); glVertex3f(0, 0.4f, 1.2f);
                        glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0.4f, 0);
                        glEnd();
                        
                        glPopMatrix();
                    }
                }
            }
            else if (Item* item = dynamic_cast<Item*>(entity))
            {
                if (/*GasCan* gasCan = */dynamic_cast<GasCan*>(item))
                {
                    glDepthMask(GL_TRUE);
                    // Render gas can as a maroon box
                    glBindTexture(GL_TEXTURE_2D, whiteTexture);
                    glColor3f(0.3f, 0.0f, 0.0f); // Maroon

                    float boxWidth = 0.4f;
                    float boxHeight = 0.6f;
                    float boxDepth = 0.3f;

                    glPushMatrix();
                    glTranslatef(entityX, 0.0f, entityZ);
                    glRotatef(-item->dirAngle, 0, 1, 0);

                    glBegin(GL_QUADS);
                    // Front face
                    glVertex3f(-boxWidth/2, 0, boxDepth/2);
                    glVertex3f(boxWidth/2, 0, boxDepth/2);
                    glVertex3f(boxWidth/2, boxHeight, boxDepth/2);
                    glVertex3f(-boxWidth/2, boxHeight, boxDepth/2);
                    // Back face
                    glVertex3f(-boxWidth/2, 0, -boxDepth/2);
                    glVertex3f(-boxWidth/2, boxHeight, -boxDepth/2);
                    glVertex3f(boxWidth/2, boxHeight, -boxDepth/2);
                    glVertex3f(boxWidth/2, 0, -boxDepth/2);
                    // Top face
                    glVertex3f(-boxWidth/2, boxHeight, -boxDepth/2);
                    glVertex3f(-boxWidth/2, boxHeight, boxDepth/2);
                    glVertex3f(boxWidth/2, boxHeight, boxDepth/2);
                    glVertex3f(boxWidth/2, boxHeight, -boxDepth/2);
                    // Bottom face (optional)
                    glVertex3f(-boxWidth/2, 0, -boxDepth/2);
                    glVertex3f(boxWidth/2, 0, -boxDepth/2);
                    glVertex3f(boxWidth/2, 0, boxDepth/2);
                    glVertex3f(-boxWidth/2, 0, boxDepth/2);
                    // Left face
                    glVertex3f(-boxWidth/2, 0, -boxDepth/2);
                    glVertex3f(-boxWidth/2, 0, boxDepth/2);
                    glVertex3f(-boxWidth/2, boxHeight, boxDepth/2);
                    glVertex3f(-boxWidth/2, boxHeight, -boxDepth/2);
                    // Right face
                    glVertex3f(boxWidth/2, 0, -boxDepth/2);
                    glVertex3f(boxWidth/2, boxHeight, -boxDepth/2);
                    glVertex3f(boxWidth/2, boxHeight, boxDepth/2);
                    glVertex3f(boxWidth/2, 0, boxDepth/2);
                    glEnd();

                    glPopMatrix();
                }
                // Render item flat on the floor
                else if (item->sh->tex.getSize().x > 0)
                {
                    unsigned int frameId = item->defaultTextureFrameIndex;
                    auto coords = sf::Vector2f{};
                    item->sh->getTexCoordsFromId(frameId, &coords.x, &coords.y);
                    float u1 = coords.x / item->sh->tex.getSize().x;
                    float v1 = coords.y / item->sh->tex.getSize().y;
                    float u2 = (coords.x + item->sh->tileSizePixelsX) / item->sh->tex.getSize().x;
                    float v2 = (coords.y + item->sh->tileSizePixelsY) / item->sh->tex.getSize().y;
                    
                    sf::Texture::bind(&item->sh->tex);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    
                    glNormal3f(0.0f, 1.0f, 0.0f); // Items on floor face UP
                    
                    glPushMatrix();
                    glTranslatef(entityX, 0.05f, entityZ);
                    glRotatef(-item->dirAngle, 0, 1, 0);

                    float x1 = -width * item->originX;
                    float x2 = x1 + width;
                    float z1 = -height * item->originY;
                    float z2 = z1 + height;
                    
                    glBegin(GL_QUADS);
                    // Top-Left of texture (-Z)
                    glTexCoord2f(u1, v1); glVertex3f(x1, 0, z1);
                    // Top-Right of texture
                    glTexCoord2f(u2, v1); glVertex3f(x2, 0, z1);
                    // Bottom-Right of texture (+Z)
                    glTexCoord2f(u2, v2); glVertex3f(x2, 0, z2);
                    // Bottom-Left of texture
                    glTexCoord2f(u1, v2); glVertex3f(x1, 0, z2);
                    glEnd();
                    
                    glPopMatrix();
                }
            }
            else if (Computer* computer = dynamic_cast<Computer*>(entity))
            {
                glDepthMask(GL_TRUE);
                // Render computer as a white box (copied from gas can rendering)
                glBindTexture(GL_TEXTURE_2D, whiteTexture);
                glColor3f(1.0f, 1.0f, 1.0f); // White

                float boxWidth = 0.4f;
                float boxHeight = 0.6f;
                float boxDepth = 0.3f;

                glPushMatrix();
                glTranslatef(entityX, 0.0f, entityZ);
                glRotatef(-computer->dirAngle, 0, 1, 0);

                glBegin(GL_QUADS);
                // Front face
                glVertex3f(-boxWidth/2, 0, boxDepth/2);
                glVertex3f(boxWidth/2, 0, boxDepth/2);
                glVertex3f(boxWidth/2, boxHeight, boxDepth/2);
                glVertex3f(-boxWidth/2, boxHeight, boxDepth/2);
                // Back face
                glVertex3f(-boxWidth/2, 0, -boxDepth/2);
                glVertex3f(-boxWidth/2, boxHeight, -boxDepth/2);
                glVertex3f(boxWidth/2, boxHeight, -boxDepth/2);
                glVertex3f(boxWidth/2, 0, -boxDepth/2);
                // Top face
                glVertex3f(-boxWidth/2, boxHeight, -boxDepth/2);
                glVertex3f(-boxWidth/2, boxHeight, boxDepth/2);
                glVertex3f(boxWidth/2, boxHeight, boxDepth/2);
                glVertex3f(boxWidth/2, boxHeight, -boxDepth/2);
                // Bottom face (optional)
                glVertex3f(-boxWidth/2, 0, -boxDepth/2);
                glVertex3f(boxWidth/2, 0, -boxDepth/2);
                glVertex3f(boxWidth/2, 0, boxDepth/2);
                glVertex3f(-boxWidth/2, 0, boxDepth/2);
                // Left face
                glVertex3f(-boxWidth/2, 0, -boxDepth/2);
                glVertex3f(-boxWidth/2, 0, boxDepth/2);
                glVertex3f(-boxWidth/2, boxHeight, boxDepth/2);
                glVertex3f(-boxWidth/2, boxHeight, -boxDepth/2);
                // Right face
                glVertex3f(boxWidth/2, 0, -boxDepth/2);
                glVertex3f(boxWidth/2, boxHeight, -boxDepth/2);
                glVertex3f(boxWidth/2, boxHeight, boxDepth/2);
                glVertex3f(boxWidth/2, 0, boxDepth/2);
                glEnd();

                glPopMatrix();
            }
            else if (Prop* prop = dynamic_cast<Prop*>(entity))
            {
                // Render prop using its sheet texture
                if (prop->sh->tex.getSize().x > 0)
                {
                    unsigned int frameId = Util::Clamp((unsigned int)prop->getDir(), 0, (prop->sh->sizeX * prop->sh->sizeY) - 1);
                    auto coords = sf::Vector2f{};
                    prop->sh->getTexCoordsFromId(frameId, &coords.x, &coords.y);
                    float u1 = coords.x / prop->sh->tex.getSize().x;
                    float v1 = coords.y / prop->sh->tex.getSize().y;
                    float u2 = (coords.x + prop->sh->tileSizePixelsX) / prop->sh->tex.getSize().x;
                    float v2 = (coords.y + prop->sh->tileSizePixelsY) / prop->sh->tex.getSize().y;
                    
                    sf::Texture::bind(&prop->sh->tex);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    
                    // Use entityZ for props (already has 8 pixel offset like actors)
                    
                    // Use entityZ for props (already has 8 pixel offset like actors)
                    glNormal3f(0.0f, 0.5f, 0.866f); // Billboards face camera
                    glBegin(GL_QUADS);
                    glTexCoord2f(u1, v2); glVertex3f(entityX + offsetX, offsetY, entityZ);
                    glTexCoord2f(u2, v2); glVertex3f(entityX + offsetX + width, offsetY, entityZ);
                    glTexCoord2f(u2, v1); glVertex3f(entityX + offsetX + width, offsetY + height, entityZ);
                    glTexCoord2f(u1, v1); glVertex3f(entityX + offsetX, offsetY + height, entityZ);
                    glEnd();
                }
            }
            else if (/*BulletProjectile* bullet = */dynamic_cast<BulletProjectile*>(entity))
            {
                // Render bullet as small white sphere
                sf::Texture::bind(nullptr);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                float bulletRadius = BulletProjectile::BulletRadius / 16.0f * entity->renderScaleX;
                
                glPushMatrix();
                glTranslatef(entityX, 0.5f, entityZ);
                
                // Simple quad billboard for bullet
                glBegin(GL_QUADS);
                glVertex3f(-bulletRadius, -bulletRadius, 0);
                glVertex3f(bulletRadius, -bulletRadius, 0);
                glVertex3f(bulletRadius, bulletRadius, 0);
                glVertex3f(-bulletRadius, bulletRadius, 0);
                glEnd();
                
                glPopMatrix();
            }
            else if (BuildingWallEntity* wall = dynamic_cast<BuildingWallEntity*>(entity))
            {
                // Render building wall with tilt support
                glDepthMask(GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, whiteTexture);
                glColor4f(0.85f, 0.85f, 0.85f, 1.0f);
                
                // Render building wall with tilt support (Simplified)
                glDepthMask(GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, whiteTexture);
                glColor4f(0.85f, 0.85f, 0.85f, 1.0f);
                
                // Entity position is the center of the wall
                float entityX = (float)(wall->posX / 16.0 - camWorldX);
                float entityY = (float)(wall->posZ / 16.0);
                float entityZ = (float)(wall->posY / 16.0 - camWorldZ);
                
                float wallLen = wall->getLength() / 16.0f;
                float wallHeight = wall->getHeight() / 16.0f;
                float thickness = BuildingWallEntity::WALL_THICKNESS / 16.0f * 2.0f; // Thicker visual

                glPushMatrix();
                glTranslatef(entityX, entityY, entityZ);
                glRotatef(-wall->dirAngle, 0, 1, 0); // Yaw
                
                // Calculate Tilt Direction (Roll)
                float fallDirX = 0, fallDirY = 0;
                wall->getFallDirection(&fallDirX, &fallDirY);
                
                float wallAngleRad = Util::ToRad(wall->dirAngle);
                float wallDirX = std::cos(wallAngleRad);
                float wallDirY = std::sin(wallAngleRad);
                
                // Cross product (2D) to determine side: WallDir x FallDir
                // If positive, fall is to the "Left" (relative to wall facing)
                // If negative, fall is to the "Right"
                float cross = wallDirX * fallDirY - wallDirY * fallDirX;
                
                float rollAngle = wall->getCurrentTiltAngle();
                if (cross > 0) rollAngle = -rollAngle;
                
                glRotatef(rollAngle, 1, 0, 0); // Roll around X axis (Length)

                // Draw Box centred at origin
                float hx = wallLen * 0.5f;
                float hy = wallHeight * 0.5f;
                float hz = thickness * 0.5f;
                
                glBegin(GL_QUADS);
                // Front (+Z)
                glNormal3f(0, 0, 1);
                glVertex3f(-hx, -hy, hz);
                glVertex3f(hx, -hy, hz);
                glVertex3f(hx, hy, hz);
                glVertex3f(-hx, hy, hz);
                // Back (-Z)
                glNormal3f(0, 0, -1);
                glVertex3f(hx, -hy, -hz);
                glVertex3f(-hx, -hy, -hz);
                glVertex3f(-hx, hy, -hz);
                glVertex3f(hx, hy, -hz);
                // Top (+Y)
                glNormal3f(0, 1, 0);
                glVertex3f(-hx, hy, hz);
                glVertex3f(hx, hy, hz);
                glVertex3f(hx, hy, -hz);
                glVertex3f(-hx, hy, -hz);
                // Bottom (-Y)
                glNormal3f(0, -1, 0);
                glVertex3f(-hx, -hy, -hz);
                glVertex3f(hx, -hy, -hz);
                glVertex3f(hx, -hy, hz);
                glVertex3f(-hx, -hy, hz);
                // Right (+X)
                glNormal3f(1, 0, 0);
                glVertex3f(hx, -hy, hz);
                glVertex3f(hx, -hy, -hz);
                glVertex3f(hx, hy, -hz);
                glVertex3f(hx, hy, hz);
                // Left (-X)
                glNormal3f(-1, 0, 0);
                glVertex3f(-hx, -hy, -hz);
                glVertex3f(-hx, -hy, hz);
                glVertex3f(-hx, hy, hz);
                glVertex3f(-hx, hy, -hz);
                
                glEnd();
                glPopMatrix();
            }
            else
            {
                // Fallback for non-actor entities - render as colored quad
                sf::Texture::bind(nullptr);
                glColor4f(1, 0, 1, 1);
                
                sf::Texture::bind(nullptr);
                glColor4f(1, 0, 1, 1);
                
                glBegin(GL_QUADS);
                glVertex3f(entityX + offsetX, offsetY, entityZ - 0.1f);
                glVertex3f(entityX + offsetX + width, offsetY, entityZ - 0.1f);
                glVertex3f(entityX + offsetX + width, offsetY + height, entityZ - 0.1f);
                glVertex3f(entityX + offsetX, offsetY + height, entityZ - 0.1f);
                glEnd();
            }
        }
    }
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void New3DRenderer::renderDebugTextAboveActors(Map* map, RendTarget& target)
{
    // Only render debug text in debug mode
#if DEBUG
    // Enable 3D rendering for billboards
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    
    // Iterate through all entities to find actors
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    for (Entity* entity : map->getEntities()) {
        Actor* actor = dynamic_cast<Actor*>(entity);
        if (actor) {
            // Create health text
            std::string healthText = "HP: " + std::to_string((int)actor->health);
            
            // Get or create the render texture for this actor
            sf::RenderTexture* textTexture = getOrCreateTextTexture(actor, healthText);
            if (!textTexture) continue;
            
            // Convert actor position from pixels to world coordinates
            float actorX = actor->posX / 16.0f;
            float actorZ = actor->posY / 16.0f;
            
            // Position text slightly above the actor's head
            float textHeight = 2.1f + actor->posZ / 16.0f;

            // Calculate billboard position (relative to camera origin)
            float textWorldX = (float)(actor->posX / 16.0 - camWorldX);
            float textWorldY = textHeight;
            float textWorldZ = (float)(actor->posY / 16.0 - camWorldZ);
            
            // Calculate camera position for billboard facing
            // Camera is at (0, height, distance) in the relative coordinate system used for rendering
            float camRelY = (float)m_camera.getEyePos().y;
            float camRelZ = m_camera.getDistance();
            
            // Calculate vector from text to camera
            float dirX = 0.0f - textWorldX;
            float dirY = camRelY - textWorldY;
            float dirZ = camRelZ - textWorldZ;
            
            // Calculate billboard rotation to face camera
            float distance = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
            if (distance > 0.001f) {
                dirX /= distance;
                dirY /= distance;
                dirZ /= distance;
            }
            
            // Fixed billboard size for consistency with the world
            float billboardSize = 0.8f;
            
            // Bind the texture
            glBindTexture(GL_TEXTURE_2D, textTexture->getTexture().getNativeHandle());
            
            // Calculate texture coordinates (flipped for OpenGL)
            float texWidth = (float)textTexture->getSize().x;
            float texHeight = (float)textTexture->getSize().y;
            float aspect = texWidth / texHeight;
            
            float halfWidth = billboardSize * 0.5f * aspect;
            float halfHeight = billboardSize * 0.5f;
            
            // Draw the billboard quad
            glBegin(GL_QUADS);
            
            // Bottom-left
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(textWorldX - halfWidth, textWorldY - halfHeight, textWorldZ);
            
            // Bottom-right
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(textWorldX + halfWidth, textWorldY - halfHeight, textWorldZ);
            
            // Top-right
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(textWorldX + halfWidth, textWorldY + halfHeight, textWorldZ);
            
            // Top-left
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(textWorldX - halfWidth, textWorldY + halfHeight, textWorldZ);
            
            glEnd();
        }
    }
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
#endif
}

sf::RenderTexture* New3DRenderer::getOrCreateTextTexture(Actor* actor, const std::string& text)
{
    auto it = debugTextCache.find(actor);
    if (it != debugTextCache.end()) {
        DebugTextTexture& textTex = *it->second;
        
        // Check if text has changed
        if (textTex.currentText != text) {
            updateTextTexture(textTex, text);
            textTex.currentText = text;
        }
        
        return &textTex.texture;
    } else {
        // Create new text texture
        auto newTextTex = std::unique_ptr<DebugTextTexture>(new DebugTextTexture());
        updateTextTexture(*newTextTex, text);
        newTextTex->currentText = text;
        
        debugTextCache[actor] = std::move(newTextTex);
        return &debugTextCache[actor]->texture;
    }
}

void New3DRenderer::updateTextTexture(DebugTextTexture& textTex, const std::string& text)
{
    // Create text with proper styling
    sf::Text sfText(text, *Fonts::OSDFont, 64); // High resolution for scaling
    sfText.setFillColor(sf::Color::Red);
    sfText.setStyle(sf::Text::Bold);
    
    // Calculate text bounds for optimal texture size
    sf::FloatRect bounds = sfText.getLocalBounds();
    int textureWidth = std::max(64, (int)bounds.width + 20);
    int textureHeight = std::max(32, (int)bounds.height + 20);
    
    // Ensure texture dimensions are powers of 2 for better OpenGL compatibility
    textureWidth = std::pow(2, std::ceil(std::log2(textureWidth)));
    textureHeight = std::pow(2, std::ceil(std::log2(textureHeight)));
    
    // Create or resize render texture
    if (!textTex.texture.create(textureWidth, textureHeight)) {
        return;
    }
    
    // Clear with transparent background
    textTex.texture.clear(sf::Color::Transparent);
    
    // Center the text in the texture
    sfText.setPosition((textureWidth - bounds.width) / 2.0f, (textureHeight - bounds.height) / 2.0f);
    
    // Draw text to texture
    textTex.texture.draw(sfText);
    textTex.texture.display();
    
    // Update sprite
    textTex.sprite.setTexture(textTex.texture.getTexture());
    textTex.sprite.setOrigin(textureWidth / 2.0f, textureHeight / 2.0f);
    textTex.isValid = true;
}

}