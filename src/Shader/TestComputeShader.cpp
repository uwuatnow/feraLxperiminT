#include "TestComputeShader.h"
#include "OpenGLComputeFunctions.h"

namespace nyaa {

TestComputeShader::TestComputeShader() : ComputeShader("test_multiply")
{
    glGenBuffers(1, &ssboInput);
    glGenBuffers(1, &ssboOutput);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboOutput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

TestComputeShader::~TestComputeShader()
{
    glDeleteBuffers(1, &ssboInput);
    glDeleteBuffers(1, &ssboOutput);
}

void TestComputeShader::runMultiplication(float a, float b, float& result)
{
    float input[2] = { a, b };

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInput);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 2, input);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboInput);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboOutput);

    use();
    dispatch(1, 1, 1);
    memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboOutput);
    float* data = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    result = data[0];
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

}