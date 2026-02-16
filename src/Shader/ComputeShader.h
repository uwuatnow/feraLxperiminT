#pragma once
#include "Game/Globals.h"
#include "OpenGLComputeFunctions.h"
#include <string>

namespace nyaa {

class ComputeShader
{
public:
    ComputeShader(std::string computeName);
    ComputeShader(std::string computeName, const std::string& sourceSourceOverride);
    ~ComputeShader();

    void use();
    void dispatch(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
    void dispatchIndirect(GLintptr indirect);

    // Uniform setters
    void setUniform1i(const std::string& name, GLint value);
    void setUniform1f(const std::string& name, GLfloat value);
    void setUniform2f(const std::string& name, GLfloat v0, GLfloat v1);
    void setUniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2);
    void setUniform4f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    void setUniform1iv(const std::string& name, GLsizei count, const GLint* value);

    // Bind image texture
    void bindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

    // Memory barrier
    static void memoryBarrier(GLbitfield barriers);

private:
    GLuint programID;
    std::string computeName;

    bool compileShader(const std::string& source);
    std::string loadShaderSource(const std::string& filename);
};

}