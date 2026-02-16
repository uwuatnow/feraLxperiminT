#pragma once
#include "Game/Globals.h"
#include "Inventory/Drug/Drug.h"

namespace nyaa {

class Cannabis : public Drug
{
public:
    Cannabis();
    
    virtual ~Cannabis();
    
    virtual void use() override;
    
    float sativaRatio;  // 0.0f to 1.0f (0% sativa to 100% sativa)
    float indicaRatio;  // 0.0f to 1.0f (0% indica to 100% indica)
};

}