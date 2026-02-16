#pragma once
#include "ComputeShader.h"

namespace nyaa {

class TestComputeShader : public ComputeShader
{
public:
    TestComputeShader();
    ~TestComputeShader();

    void runMultiplication(float a, float b, float& result);

private:
    GLuint ssboInput;
    GLuint ssboOutput;
};

}