#include "OpenGLComputeFunctions.h"
#include <Windows.h> // for wglGetProcAddress
#include <cassert>

bool initialized = false;

PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute = nullptr;
PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect = nullptr;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = nullptr;
PFNGLMEMORYBARRIERPROC glMemoryBarrier = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLBUFFERSUBDATAPROC glBufferSubData = nullptr;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = nullptr;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr;
PFNGLMAPBUFFERPROC glMapBuffer = nullptr;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM2FPROC glUniform2f = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLUNIFORM1IVPROC glUniform1iv = nullptr;

void InitializeOpenGLComputeFunctions()
{
    if (initialized) return;
    initialized = true;
    
    // Shader and Program functions
    glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glCreateShader")));
    assert(glCreateShader != nullptr);
    glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(reinterpret_cast<void*>(wglGetProcAddress("glShaderSource")));
    assert(glShaderSource != nullptr);
    glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glCompileShader")));
    assert(glCompileShader != nullptr);
    glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGetShaderiv")));
    assert(glGetShaderiv != nullptr);
    glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGetShaderInfoLog")));
    assert(glGetShaderInfoLog != nullptr);
    glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glDeleteShader")));
    assert(glDeleteShader != nullptr);

    glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(reinterpret_cast<void*>(wglGetProcAddress("glCreateProgram")));
    assert(glCreateProgram != nullptr);
    glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glAttachShader")));
    assert(glAttachShader != nullptr);
    glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(reinterpret_cast<void*>(wglGetProcAddress("glLinkProgram")));
    assert(glLinkProgram != nullptr);
    glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGetProgramiv")));
    assert(glGetProgramiv != nullptr);
    glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGetProgramInfoLog")));
    assert(glGetProgramInfoLog != nullptr);
    glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUseProgram")));
    assert(glUseProgram != nullptr);
    glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(reinterpret_cast<void*>(wglGetProcAddress("glDeleteProgram")));
    assert(glDeleteProgram != nullptr);

    // Compute dispatch
    glDispatchCompute = reinterpret_cast<PFNGLDISPATCHCOMPUTEPROC>(reinterpret_cast<void*>(wglGetProcAddress("glDispatchCompute")));
    assert(glDispatchCompute != nullptr);
    glDispatchComputeIndirect = reinterpret_cast<PFNGLDISPATCHCOMPUTEINDIRECTPROC>(reinterpret_cast<void*>(wglGetProcAddress("glDispatchComputeIndirect")));
    assert(glDispatchComputeIndirect != nullptr);

    // Image and buffer functions
    glBindImageTexture = reinterpret_cast<PFNGLBINDIMAGETEXTUREPROC>(reinterpret_cast<void*>(wglGetProcAddress("glBindImageTexture")));
    assert(glBindImageTexture != nullptr);

    // Memory barrier
    glMemoryBarrier = reinterpret_cast<PFNGLMEMORYBARRIERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glMemoryBarrier")));
    assert(glMemoryBarrier != nullptr);

    // Buffer functions
    glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGenBuffers")));
    assert(glGenBuffers != nullptr);
    glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glBindBuffer")));
    assert(glBindBuffer != nullptr);
    glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(reinterpret_cast<void*>(wglGetProcAddress("glBufferData")));
    assert(glBufferData != nullptr);
    glBufferSubData = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(reinterpret_cast<void*>(wglGetProcAddress("glBufferSubData")));
    assert(glBufferSubData != nullptr);
    glGetBufferSubData = reinterpret_cast<PFNGLGETBUFFERSUBDATAPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGetBufferSubData")));
    assert(glGetBufferSubData != nullptr);
    glBindBufferBase = reinterpret_cast<PFNGLBINDBUFFERBASEPROC>(reinterpret_cast<void*>(wglGetProcAddress("glBindBufferBase")));
    assert(glBindBufferBase != nullptr);
    glMapBuffer = reinterpret_cast<PFNGLMAPBUFFERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glMapBuffer")));
    assert(glMapBuffer != nullptr);
    glUnmapBuffer = reinterpret_cast<PFNGLUNMAPBUFFERPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUnmapBuffer")));
    assert(glUnmapBuffer != nullptr);
    glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(reinterpret_cast<void*>(wglGetProcAddress("glDeleteBuffers")));
    assert(glDeleteBuffers != nullptr);

    // Uniform functions
    glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(reinterpret_cast<void*>(wglGetProcAddress("glGetUniformLocation")));
    assert(glGetUniformLocation != nullptr);
    glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUniform1i")));
    assert(glUniform1i != nullptr);
    glUniform1f = reinterpret_cast<PFNGLUNIFORM1FPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUniform1f")));
    assert(glUniform1f != nullptr);
    glUniform2f = reinterpret_cast<PFNGLUNIFORM2FPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUniform2f")));
    assert(glUniform2f != nullptr);
    glUniform3f = reinterpret_cast<PFNGLUNIFORM3FPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUniform3f")));
    assert(glUniform3f != nullptr);
    glUniform4f = reinterpret_cast<PFNGLUNIFORM4FPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUniform4f")));
    assert(glUniform4f != nullptr);
    glUniform1iv = reinterpret_cast<PFNGLUNIFORM1IVPROC>(reinterpret_cast<void*>(wglGetProcAddress("glUniform1iv")));
    assert(glUniform1iv != nullptr);
}