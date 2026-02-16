#pragma once

#include <SFML/OpenGL.hpp>

// OpenGL constants for compute shaders
#define GL_COMPUTE_SHADER 0x91B9
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000
#define GL_ALL_BARRIER_BITS 0xFFFFFFFF
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_READ_ONLY 0x88B8

// Shader compilation constants
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

// OpenGL types
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// OpenGL function pointers for compute shaders

// Shader and Program functions
typedef GLuint (WINAPI *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (WINAPI *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (WINAPI *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (WINAPI *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (WINAPI *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (WINAPI *PFNGLDELETESHADERPROC)(GLuint shader);

typedef GLuint (WINAPI *PFNGLCREATEPROGRAMPROC)(void);
typedef void (WINAPI *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (WINAPI *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (WINAPI *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (WINAPI *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (WINAPI *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (WINAPI *PFNGLDELETEPROGRAMPROC)(GLuint program);

// Compute dispatch
typedef void (WINAPI *PFNGLDISPATCHCOMPUTEPROC)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (WINAPI *PFNGLDISPATCHCOMPUTEINDIRECTPROC)(GLintptr indirect);

// Image and buffer functions
typedef void (WINAPI *PFNGLBINDIMAGETEXTUREPROC)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

// Memory barrier
typedef void (WINAPI *PFNGLMEMORYBARRIERPROC)(GLbitfield barriers);

// Buffer functions
typedef void (WINAPI *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (WINAPI *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (WINAPI *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (WINAPI *PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (WINAPI *PFNGLGETBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
typedef void (WINAPI *PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void* (WINAPI *PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
typedef GLboolean (WINAPI *PFNGLUNMAPBUFFERPROC)(GLenum target);
typedef void (WINAPI *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);

// Uniform functions
typedef GLint (WINAPI *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (WINAPI *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (WINAPI *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (WINAPI *PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (WINAPI *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (WINAPI *PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (WINAPI *PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint *value);

// Static function pointers
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
extern PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
extern PFNGLMEMORYBARRIERPROC glMemoryBarrier;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
extern PFNGLMAPBUFFERPROC glMapBuffer;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORM1IVPROC glUniform1iv;

// Function to initialize all pointers
void InitializeOpenGLComputeFunctions();