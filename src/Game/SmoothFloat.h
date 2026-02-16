#pragma once
#include "Game/Globals.h"
#include <array>

namespace nyaa {

class SmoothFloat
{
    std::array<float, 10> hist{};
    int idx = 0;
    bool filled = false;
    float cached;

public:
    SmoothFloat(float v = 0.0f) {
        hist.fill(v);
        filled = true;
    }

    SmoothFloat& operator=(float v) {
        hist[idx] = v;
        idx = (idx + 1) % 10;
        if (idx == 0) filled = true;        
        int n = filled ? 10 : idx;
        float s = 0.0f;
        for (int i = 0; i < n; i++) s += hist[i];
        cached = s / n;
        return *this;
    }

    operator float() const {
        return cached;
    }
};

}
