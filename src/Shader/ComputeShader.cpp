#include "ComputeShader.h"
#include "OpenGLComputeFunctions.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

namespace nyaa {

ComputeShader::ComputeShader(std::string computeName)
    : programID(0), computeName(computeName)
{
    std::string source = loadShaderSource(Util::Format("%s%s.compute", Game::ResDir, computeName.c_str()));
    if (!compileShader(source)) {
        std::cerr << "Failed to compile compute shader: " << computeName << std::endl;
        assert(false);
    }
}

ComputeShader::ComputeShader(std::string computeName, const std::string& sourceSourceOverride)
    : programID(0), computeName(computeName)
{
    if (!compileShader(sourceSourceOverride)) {
        std::cerr << "Failed to compile compute shader from source: " << computeName << std::endl;
        assert(false);
    }
}

ComputeShader::~ComputeShader()
{
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void ComputeShader::use()
{
    glUseProgram(programID);
}

void ComputeShader::dispatch(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
{
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
}

void ComputeShader::dispatchIndirect(GLintptr indirect)
{
    glDispatchComputeIndirect(indirect);
}

void ComputeShader::setUniform1i(const std::string& name, GLint value)
{
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void ComputeShader::setUniform1f(const std::string& name, GLfloat value)
{
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void ComputeShader::setUniform2f(const std::string& name, GLfloat v0, GLfloat v1)
{
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        glUniform2f(location, v0, v1);
    }
}

void ComputeShader::setUniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        glUniform3f(location, v0, v1, v2);
    }
}

void ComputeShader::setUniform4f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        glUniform4f(location, v0, v1, v2, v3);
    }
}

void ComputeShader::setUniform1iv(const std::string& name, GLsizei count, const GLint* value)
{
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        glUniform1iv(location, count, value);
    }
}

void ComputeShader::bindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
{
    glBindImageTexture(unit, texture, level, layered, layer, access, format);
}

void ComputeShader::memoryBarrier(GLbitfield barriers)
{
    glMemoryBarrier(barriers);
}

bool ComputeShader::compileShader(const std::string& source)
{
    // Create compute shader
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    const char* sourcePtr = source.c_str();
    glShaderSource(computeShader, 1, &sourcePtr, nullptr);
    glCompileShader(computeShader);

    // Check compilation status
    GLint success;
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
        std::cerr << "Compute shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(computeShader);
        return false;
    }

    // Create program
    programID = glCreateProgram();
    glAttachShader(programID, computeShader);
    glLinkProgram(programID);

    // Check linking status
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Compute shader linking failed:\n" << infoLog << std::endl;
        glDeleteShader(computeShader);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

    // Cleanup
    glDeleteShader(computeShader);
    return true;
}

std::string ComputeShader::loadShaderSource(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open compute shader file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

}