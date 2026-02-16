#pragma once

#include <Windows.h>
#include <GL/gl.h>
#include <cstdint>

// =============================================================================
// ModernGLRenderer - Phase 0: OpenGL 4.3+ Function Pointer Setup
// =============================================================================
// 
// This file contains all function pointers for modern OpenGL 4.3+ functions
// that will be used by the ModernGLRenderer. These are loaded ONCE at startup
// using wglGetProcAddress and stored in function pointers for use throughout
// the renderer.
//
// IMPORTANT: Do NOT call these directly - use through the GL namespace below
//

// =============================================================================
// OpenGL Type Definitions (for completeness)
// =============================================================================

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef struct __GLsync* GLsync;

// =============================================================================
// Additional OpenGL Constants
// =============================================================================

#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#endif

#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#endif

#ifndef GL_DEBUG_SEVERITY_HIGH
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#endif

#ifndef GL_DEBUG_SEVERITY_MEDIUM
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#endif

#ifndef GL_DEBUG_SEVERITY_LOW
#define GL_DEBUG_SEVERITY_LOW 0x9148
#endif

#ifndef GL_DEBUG_TYPE_ERROR
#define GL_DEBUG_TYPE_ERROR 0x824C
#endif

#ifndef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#endif

#ifndef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#endif

#ifndef GL_DEBUG_TYPE_PORTABILITY
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#endif

#ifndef GL_DEBUG_TYPE_PERFORMANCE
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#endif

#ifndef GL_DEBUG_TYPE_OTHER
#define GL_DEBUG_TYPE_OTHER 0x8251
#endif

#ifndef GL_DEBUG_SOURCE_API
#define GL_DEBUG_SOURCE_API 0x8246
#endif

#ifndef GL_DEBUG_SOURCE_WINDOW_SYSTEM
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#endif

#ifndef GL_DEBUG_SOURCE_SHADER_COMPILER
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#endif

#ifndef GL_DEBUG_SOURCE_THIRD_PARTY
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#endif

#ifndef GL_DEBUG_SOURCE_APPLICATION
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#endif

#ifndef GL_DEBUG_SOURCE_OTHER
#define GL_DEBUG_SOURCE_OTHER 0x824B
#endif

#ifndef GL_BUFFER_STORAGE_FLAGS
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_MAP_COHERENT_BIT 0x0080
#define GL_DYNAMIC_STORAGE_BIT 0x0100
#endif

#ifndef GL_SYNC_GPU_COMMANDS_COMPLETE
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#endif

#ifndef GL_TIMEOUT_IGNORED
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFFull
#endif

#ifndef GL_QUERY_RESULT
#define GL_QUERY_RESULT 0x8866
#endif

#ifndef GL_QUERY_RESULT_AVAILABLE
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#endif

#ifndef GL_TIME_ELAPSED
#define GL_TIME_ELAPSED 0x88BF
#endif

#ifndef GL_TIMESTAMP
#define GL_TIMESTAMP 0x8E28
#endif

#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif

#ifndef GL_SHADER_STORAGE_BUFFER
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#endif

#ifndef GL_UNIFORM_BUFFER
#define GL_UNIFORM_BUFFER 0x8A11
#endif

#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif

#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif

#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT 0x8D00
#endif

#ifndef GL_DEPTH_STENCIL_ATTACHMENT
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#endif

#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif

#ifndef GL_TEXTURE_2D_ARRAY
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#endif

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif

#ifndef GL_RGBA32F
#define GL_RGBA32F 0x8814
#endif

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif

#ifndef GL_UNIFORM_BUFFER
#define GL_UNIFORM_BUFFER 0x8A11
#endif

#ifndef GL_SHADER_STORAGE_BUFFER
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#endif

#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif

#ifndef GL_DYNAMIC_DRAW
#define GL_DYNAMIC_DRAW 0x88E8
#endif

#ifndef GL_STREAM_DRAW
#define GL_STREAM_DRAW 0x88E0
#endif

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_TRIANGLES
#define GL_TRIANGLES 0x0004
#endif

#ifndef GL_TRIANGLE_STRIP
#define GL_TRIANGLE_STRIP 0x0005
#endif

#ifndef GL_TRIANGLE_FAN
#define GL_TRIANGLE_FAN 0x0006
#endif

#ifndef GL_LINES
#define GL_LINES 0x0001
#endif

#ifndef GL_LINE_STRIP
#define GL_LINE_STRIP 0x0003
#endif

#ifndef GL_POINTS
#define GL_POINTS 0x0000
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif

#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT 0x00004000
#endif

#ifndef GL_DEPTH_BUFFER_BIT
#define GL_DEPTH_BUFFER_BIT 0x00000100
#endif

#ifndef GL_STENCIL_BUFFER_BIT
#define GL_STENCIL_BUFFER_BIT 0x00000400
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#ifndef GL_SAMPLE_ALPHA_TO_COVERAGE
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#endif

#ifndef GL_SAMPLE_COVERAGE
#define GL_SAMPLE_COVERAGE 0x80A0
#endif

#ifndef GL_FUNC_ADD
#define GL_FUNC_ADD 0x8006
#endif

#ifndef GL_FUNC_SUBTRACT
#define GL_FUNC_SUBTRACT 0x800A
#endif

#ifndef GL_FUNC_REVERSE_SUBTRACT
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#endif

#ifndef GL_MIN
#define GL_MIN 0x8007
#endif

#ifndef GL_MAX
#define GL_MAX 0x8008
#endif

#ifndef GL_ZERO
#define GL_ZERO 0
#endif

#ifndef GL_ONE
#define GL_ONE 1
#endif

#ifndef GL_SRC_COLOR
#define GL_SRC_COLOR 0x0300
#endif

#ifndef GL_ONE_MINUS_SRC_COLOR
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#endif

#ifndef GL_SRC_ALPHA
#define GL_SRC_ALPHA 0x0302
#endif

#ifndef GL_ONE_MINUS_SRC_ALPHA
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#endif

#ifndef GL_DST_ALPHA
#define GL_DST_ALPHA 0x0304
#endif

#ifndef GL_ONE_MINUS_DST_ALPHA
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#endif

#ifndef GL_DST_COLOR
#define GL_DST_COLOR 0x0306
#endif

#ifndef GL_ONE_MINUS_DST_COLOR
#define GL_ONE_MINUS_DST_COLOR 0x0307
#endif

#ifndef GL_LESS
#define GL_LESS 0x0201
#endif

#ifndef GL_LEQUAL
#define GL_LEQUAL 0x0203
#endif

#ifndef GL_GEQUAL
#define GL_GEQUAL 0x0206
#endif

#ifndef GL_GREATER
#define GL_GREATER 0x0204
#endif

#ifndef GL_EQUAL
#define GL_EQUAL 0x0202
#endif

#ifndef GL_NOTEQUAL
#define GL_NOTEQUAL 0x0205
#endif

#ifndef GL_ALWAYS
#define GL_ALWAYS 0x0207
#endif

#ifndef GL_NEVER
#define GL_NEVER 0x0200
#endif

#ifndef GL_CW
#define GL_CW 0x0900
#endif

#ifndef GL_CCW
#define GL_CCW 0x0901
#endif

#ifndef GL_FRONT
#define GL_FRONT 0x0404
#endif

#ifndef GL_BACK
#define GL_BACK 0x0405
#endif

#ifndef GL_FRONT_AND_BACK
#define GL_FRONT_AND_BACK 0x0408
#endif

#ifndef GL_CULL_FACE
#define GL_CULL_FACE 0x0B44
#endif

#ifndef GL_DEPTH_TEST
#define GL_DEPTH_TEST 0x0B71
#endif

#ifndef GL_STENCIL_TEST
#define GL_STENCIL_TEST 0x0B90
#endif

#ifndef GL_BLEND
#define GL_BLEND 0x0BE2
#endif

#ifndef GL_SCISSOR_TEST
#define GL_SCISSOR_TEST 0x0C11
#endif

#ifndef GL_TEXTURE_WRAP_S
#define GL_TEXTURE_WRAP_S 0x2802
#endif

#ifndef GL_TEXTURE_WRAP_T
#define GL_TEXTURE_WRAP_T 0x2803
#endif

#ifndef GL_TEXTURE_MIN_FILTER
#define GL_TEXTURE_MIN_FILTER 0x2801
#endif

#ifndef GL_TEXTURE_MAG_FILTER
#define GL_TEXTURE_MAG_FILTER 0x2800
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_CLAMP_TO_BORDER
#define GL_CLAMP_TO_BORDER 0x812D
#endif

#ifndef GL_REPEAT
#define GL_REPEAT 0x2901
#endif

#ifndef GL_MIRRORED_REPEAT
#define GL_MIRRORED_REPEAT 0x8370
#endif

#ifndef GL_NEAREST
#define GL_NEAREST 0x2600
#endif

#ifndef GL_LINEAR
#define GL_LINEAR 0x2601
#endif

#ifndef GL_NEAREST_MIPMAP_NEAREST
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#endif

#ifndef GL_LINEAR_MIPMAP_NEAREST
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#endif

#ifndef GL_NEAREST_MIPMAP_LINEAR
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#endif

#ifndef GL_LINEAR_MIPMAP_LINEAR
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#endif

#ifndef GL_FALSE
#define GL_FALSE 0
#endif

#ifndef GL_TRUE
#define GL_TRUE 1
#endif

#ifndef GL_NONE
#define GL_NONE 0
#endif

#ifndef GL_NO_ERROR
#define GL_NO_ERROR 0
#endif

#ifndef GL_INVALID_ENUM
#define GL_INVALID_ENUM 0x0500
#endif

#ifndef GL_INVALID_VALUE
#define GL_INVALID_VALUE 0x0501
#endif

#ifndef GL_INVALID_OPERATION
#define GL_INVALID_OPERATION 0x0502
#endif

#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW 0x0503
#endif

#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW 0x0504
#endif

#ifndef GL_OUT_OF_MEMORY
#define GL_OUT_OF_MEMORY 0x0505
#endif

#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#endif

#ifndef GL_CONTEXT_LOST
#define GL_CONTEXT_LOST 0x0507
#endif

#ifndef GL_TEXTURE_COMPARE_MODE
#define GL_TEXTURE_COMPARE_MODE 0x884C
#endif

#ifndef GL_TEXTURE_COMPARE_FUNC
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#endif

#ifndef GL_COMPARE_REF_TO_TEXTURE
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#endif

#ifndef GL_DEPTH_COMPONENT
#define GL_DEPTH_COMPONENT 0x1902
#endif

#ifndef GL_DEPTH_COMPONENT16
#define GL_DEPTH_COMPONENT16 0x81A5
#endif

#ifndef GL_DEPTH_COMPONENT24
#define GL_DEPTH_COMPONENT24 0x81A6
#endif

#ifndef GL_DEPTH_COMPONENT32
#define GL_DEPTH_COMPONENT32 0x81A7
#endif

#ifndef GL_DEPTH_COMPONENT32F
#define GL_DEPTH_COMPONENT32F 0x8CAC
#endif

#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif

#ifndef GL_STENCIL_INDEX8
#define GL_STENCIL_INDEX8 0x8D48
#endif

#ifndef GL_RED
#define GL_RED 0x1903
#endif

#ifndef GL_RG
#define GL_RG 0x8227
#endif

#ifndef GL_RGB
#define GL_RGB 0x1907
#endif

#ifndef GL_RGBA
#define GL_RGBA 0x1908
#endif

#ifndef GL_R8
#define GL_R8 0x8229
#endif

#ifndef GL_RG8
#define GL_RG8 0x822B
#endif

#ifndef GL_RGB8
#define GL_RGB8 0x8051
#endif

#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif

#ifndef GL_R16F
#define GL_R16F 0x822D
#endif

#ifndef GL_RG16F
#define GL_RG16F 0x822F
#endif

#ifndef GL_RGB16F
#define GL_RGB16F 0x881B
#endif

#ifndef GL_R32F
#define GL_R32F 0x822E
#endif

#ifndef GL_RG32F
#define GL_RG32F 0x8230
#endif

#ifndef GL_RGB32F
#define GL_RGB32F 0x8815
#endif

#ifndef GL_UNSIGNED_BYTE
#define GL_UNSIGNED_BYTE 0x1401
#endif

#ifndef GL_UNSIGNED_SHORT
#define GL_UNSIGNED_SHORT 0x1403
#endif

#ifndef GL_UNSIGNED_INT
#define GL_UNSIGNED_INT 0x1405
#endif

#ifndef GL_FLOAT
#define GL_FLOAT 0x1406
#endif

#ifndef GL_HALF_FLOAT
#define GL_HALF_FLOAT 0x140B
#endif

#ifndef GL_BYTE
#define GL_BYTE 0x1400
#endif

#ifndef GL_SHORT
#define GL_SHORT 0x1402
#endif

#ifndef GL_INT
#define GL_INT 0x1404
#endif

#ifndef GL_WAIT_FAILED
#define GL_WAIT_FAILED 0x911D
#endif

#ifndef GL_ALREADY_SIGNALED
#define GL_ALREADY_SIGNALED 0x911A
#endif

#ifndef GL_TIMEOUT_EXPIRED
#define GL_TIMEOUT_EXPIRED 0x911B
#endif

#ifndef GL_CONDITION_SATISFIED
#define GL_CONDITION_SATISFIED 0x911C
#endif

#ifndef GL_SYNC_STATUS
#define GL_SYNC_STATUS 0x9114
#endif

#ifndef GL_SIGNALED
#define GL_SIGNALED 0x9119
#endif

#ifndef GL_UNSIGNALED
#define GL_UNSIGNALED 0x9118
#endif

#ifndef GL_SYNC_FLUSH_COMMANDS_BIT
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#endif

// =============================================================================
// Function Pointer Typedefs
// =============================================================================

// Vertex Array Object functions
typedef void (WINAPI *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void (WINAPI *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);
typedef void (WINAPI *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (WINAPI *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (WINAPI *PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (WINAPI *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (WINAPI *PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
typedef void (WINAPI *PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);

// Buffer functions
typedef void (WINAPI *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (WINAPI *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
typedef void (WINAPI *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (WINAPI *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (WINAPI *PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
typedef void (WINAPI *PFNGLBUFFERSTORAGEPROC)(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void* (WINAPI *PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
typedef void* (WINAPI *PFNGLMAPBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (WINAPI *PFNGLUNMAPBUFFERPROC)(GLenum target);
typedef void (WINAPI *PFNGLGETBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, void* data);
typedef void (WINAPI *PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void (WINAPI *PFNGLBINDBUFFERRANGEPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);

// DSA (Direct State Access) Buffer functions
typedef void (WINAPI *PFNGLCREATEBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (WINAPI *PFNGLNAMEDBUFFERSTORAGEPROC)(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void (WINAPI *PFNGLNAMEDBUFFERSUBDATAPROC)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef void* (WINAPI *PFNGLMAPNAMEDBUFFERRANGEPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (WINAPI *PFNGLUNMAPNAMEDBUFFERPROC)(GLuint buffer);
typedef void (WINAPI *PFNGLGETNAMEDBUFFERSUBDATAPROC)(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);

// Shader functions
typedef GLuint (WINAPI *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (WINAPI *PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (WINAPI *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void (WINAPI *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (WINAPI *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void (WINAPI *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (WINAPI *PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);

// Program functions
typedef GLuint (WINAPI *PFNGLCREATEPROGRAMPROC)(void);
typedef void (WINAPI *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (WINAPI *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (WINAPI *PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (WINAPI *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (WINAPI *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (WINAPI *PFNGLVALIDATEPROGRAMPROC)(GLuint program);
typedef void (WINAPI *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void (WINAPI *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

// Uniform functions
typedef GLint (WINAPI *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (WINAPI *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (WINAPI *PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (WINAPI *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (WINAPI *PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (WINAPI *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (WINAPI *PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
typedef void (WINAPI *PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
typedef void (WINAPI *PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (WINAPI *PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (WINAPI *PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (WINAPI *PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (WINAPI *PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (WINAPI *PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (WINAPI *PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (WINAPI *PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (WINAPI *PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (WINAPI *PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (WINAPI *PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (WINAPI *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// Uniform block functions
typedef GLuint (WINAPI *PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint program, const GLchar* uniformBlockName);
typedef void (WINAPI *PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
typedef void (WINAPI *PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
typedef void (WINAPI *PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);

// Texture functions
typedef void (WINAPI *PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
typedef void (WINAPI *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void (WINAPI *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (WINAPI *PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (WINAPI *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
typedef void (WINAPI *PFNGLTEXIMAGE3DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
typedef void (WINAPI *PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void (WINAPI *PFNGLTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
typedef void (WINAPI *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (WINAPI *PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
typedef void (WINAPI *PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void (WINAPI *PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);

// Sampler functions
typedef void (WINAPI *PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint* samplers);
typedef void (WINAPI *PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint* samplers);
typedef void (WINAPI *PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
typedef void (WINAPI *PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void (WINAPI *PFNGLSAMPLERPARAMETERFPROC)(GLuint sampler, GLenum pname, GLfloat param);

// Framebuffer functions
typedef void (WINAPI *PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef void (WINAPI *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
typedef void (WINAPI *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (WINAPI *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (WINAPI *PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (WINAPI *PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef GLenum (WINAPI *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (WINAPI *PFNGLBLITFRAMEBUFFERPROC)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

// Renderbuffer functions
typedef void (WINAPI *PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint* renderbuffers);
typedef void (WINAPI *PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint* renderbuffers);
typedef void (WINAPI *PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
typedef void (WINAPI *PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (WINAPI *PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

// Sync functions
typedef GLsync (WINAPI *PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
typedef void (WINAPI *PFNGLDELETESYNCPROC)(GLsync sync);
typedef GLenum (WINAPI *PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (WINAPI *PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (WINAPI *PFNGLGETSYNCIVPROC)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);

// Query functions
typedef void (WINAPI *PFNGLGENQUERIESPROC)(GLsizei n, GLuint* ids);
typedef void (WINAPI *PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint* ids);
typedef void (WINAPI *PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
typedef void (WINAPI *PFNGLENDQUERYPROC)(GLenum target);
typedef void (WINAPI *PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef void (WINAPI *PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint* params);
typedef void (WINAPI *PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint* params);
typedef void (WINAPI *PFNGLGETQUERYOBJECTI64VPROC)(GLuint id, GLenum pname, GLint64* params);
typedef void (WINAPI *PFNGLGETQUERYOBJECTUI64VPROC)(GLuint id, GLenum pname, GLuint64* params);
typedef void (WINAPI *PFNGLQUERYCOUNTERPROC)(GLuint id, GLenum target);

// Debug callback type (must be defined before the function pointer types that use it)
typedef void (WINAPI *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

// Debug functions
typedef void (WINAPI *PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void* userParam);
typedef void (WINAPI *PFNGLDEBUGMESSAGECONTROLPROC)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
typedef void (WINAPI *PFNGLDEBUGMESSAGEINSERTPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
typedef void (WINAPI *PFNGLGETDEBUGMESSAGELOGPROC)(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);
typedef void (WINAPI *PFNGLPUSHDEBUGGROUPPROC)(GLenum source, GLuint id, GLsizei length, const GLchar* message);
typedef void (WINAPI *PFNGLPOPDEBUGGROUPPROC)(void);
typedef void (WINAPI *PFNGLOBJECTLABELPROC)(GLenum identifier, GLuint name, GLsizei length, const GLchar* label);
typedef void (WINAPI *PFNGLGETOBJECTLABELPROC)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label);
typedef void (WINAPI *PFNGLOBJECTPTRLABELPROC)(GLenum identifier, const void* ptr, GLsizei length, const GLchar* label);
typedef void (WINAPI *PFNGLGETOBJECTPTRLABELPROC)(GLenum identifier, const void* ptr, GLsizei bufSize, GLsizei* length, GLchar* label);

// Draw functions
typedef void (WINAPI *PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (WINAPI *PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (WINAPI *PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
typedef void (WINAPI *PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void (WINAPI *PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
typedef void (WINAPI *PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void (WINAPI *PFNGLMULTIDRAWARRAYSPROC)(GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount);
typedef void (WINAPI *PFNGLMULTIDRAWELEMENTSPROC)(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount);
typedef void (WINAPI *PFNGLMULTIDRAWARRAYSINDIRECTPROC)(GLenum mode, const void* indirect, GLsizei drawcount, GLsizei stride);
typedef void (WINAPI *PFNGLMULTIDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride);

// Compute shader functions
typedef void (WINAPI *PFNGLDISPATCHCOMPUTEPROC)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (WINAPI *PFNGLDISPATCHCOMPUTEINDIRECTPROC)(GLintptr indirect);
typedef void (WINAPI *PFNGLMEMORYBARRIERPROC)(GLbitfield barriers);

// Image/texture binding functions
typedef void (WINAPI *PFNGLBINDIMAGETEXTUREPROC)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

// Capability and state functions
typedef void (WINAPI *PFNGLENABLEPROC)(GLenum cap);
typedef void (WINAPI *PFNGLDISABLEPROC)(GLenum cap);
typedef void (WINAPI *PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
typedef void (WINAPI *PFNGLGETFLOATVPROC)(GLenum pname, GLfloat* data);
typedef const GLubyte* (WINAPI *PFNGLGETSTRINGPROC)(GLenum name);
typedef void (WINAPI *PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);
typedef void (WINAPI *PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (WINAPI *PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (WINAPI *PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (WINAPI *PFNGLCLEARDEPTHPROC)(GLdouble depth);
typedef void (WINAPI *PFNGLCLEARPROC)(GLbitfield mask);
typedef void (WINAPI *PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (WINAPI *PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef void (WINAPI *PFNGLDEPTHRANGEPROC)(GLdouble near, GLdouble far);
typedef void (WINAPI *PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (WINAPI *PFNGLBLENDFUNCSEPARATEPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (WINAPI *PFNGLBLENDEQUATIONPROC)(GLenum mode);
typedef void (WINAPI *PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
typedef void (WINAPI *PFNGLCULLFACEPROC)(GLenum mode);
typedef void (WINAPI *PFNGLFRONTFACEPROC)(GLenum mode);
typedef void (WINAPI *PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
typedef void (WINAPI *PFNGLPOLYGONOFFSETPROC)(GLfloat factor, GLfloat units);
typedef void (WINAPI *PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
typedef void (WINAPI *PFNGLSTENCILMASKPROC)(GLuint mask);
typedef void (WINAPI *PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);

// DSA (Direct State Access) functions - OpenGL 4.5+
typedef void (WINAPI *PFNGLCREATETEXTURESPROC)(GLenum target, GLsizei n, GLuint* textures);
typedef void (WINAPI *PFNGLTEXTURESTORAGE2DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (WINAPI *PFNGLTEXTURESUBIMAGE2DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void (WINAPI *PFNGLBINDTEXTUREUNITPROC)(GLuint unit, GLuint texture);
typedef void (WINAPI *PFNGLTEXTUREPARAMETERIPROC)(GLuint texture, GLenum pname, GLint param);
typedef void (WINAPI *PFNGLGENERATETEXTUREMIPMAPPROC)(GLuint texture);
typedef void (WINAPI *PFNGLCREATEFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef void (WINAPI *PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (WINAPI *PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)(GLuint framebuffer, GLenum buf);
typedef void (WINAPI *PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC)(GLuint framebuffer, GLenum src);
typedef GLenum (WINAPI *PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)(GLuint framebuffer, GLenum target);
typedef void (WINAPI *PFNGLCREATEVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void (WINAPI *PFNGLENABLEVERTEXARRAYATTRIBPROC)(GLuint vaobj, GLuint index);
typedef void (WINAPI *PFNGLDISABLEVERTEXARRAYATTRIBPROC)(GLuint vaobj, GLuint index);
typedef void (WINAPI *PFNGLVERTEXARRAYVERTEXBUFFERPROC)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (WINAPI *PFNGLVERTEXARRAYATTRIBFORMATPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (WINAPI *PFNGLVERTEXARRAYATTRIBBINDINGPROC)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);

// =============================================================================
// Function Pointer Declarations (External)
// =============================================================================

namespace nyaa {
namespace GL {

// Vertex Array Object functions
extern PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC BindVertexArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC DisableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer;
extern PFNGLVERTEXATTRIBIPOINTERPROC VertexAttribIPointer;
extern PFNGLVERTEXATTRIBDIVISORPROC VertexAttribDivisor;

// Buffer functions
extern PFNGLGENBUFFERSPROC GenBuffers;
extern PFNGLDELETEBUFFERSPROC DeleteBuffers;
extern PFNGLBINDBUFFERPROC BindBuffer;
extern PFNGLBUFFERDATAPROC BufferData;
extern PFNGLBUFFERSUBDATAPROC BufferSubData;
extern PFNGLBUFFERSTORAGEPROC BufferStorage;
extern PFNGLMAPBUFFERPROC MapBuffer;
extern PFNGLMAPBUFFERRANGEPROC MapBufferRange;
extern PFNGLUNMAPBUFFERPROC UnmapBuffer;
extern PFNGLGETBUFFERSUBDATAPROC GetBufferSubData;
extern PFNGLBINDBUFFERBASEPROC BindBufferBase;
extern PFNGLBINDBUFFERRANGEPROC BindBufferRange;

// DSA (Direct State Access) Buffer functions
extern PFNGLCREATEBUFFERSPROC CreateBuffers;
extern PFNGLNAMEDBUFFERSTORAGEPROC NamedBufferStorage;
extern PFNGLNAMEDBUFFERSUBDATAPROC NamedBufferSubData;
extern PFNGLMAPNAMEDBUFFERRANGEPROC MapNamedBufferRange;
extern PFNGLUNMAPNAMEDBUFFERPROC UnmapNamedBuffer;
extern PFNGLGETNAMEDBUFFERSUBDATAPROC GetNamedBufferSubData;

// Shader functions
extern PFNGLCREATESHADERPROC CreateShader;
extern PFNGLDELETESHADERPROC DeleteShader;
extern PFNGLSHADERSOURCEPROC ShaderSource;
extern PFNGLCOMPILESHADERPROC CompileShader;
extern PFNGLGETSHADERIVPROC GetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC GetShaderSource;

// Program functions
extern PFNGLCREATEPROGRAMPROC CreateProgram;
extern PFNGLDELETEPROGRAMPROC DeleteProgram;
extern PFNGLATTACHSHADERPROC AttachShader;
extern PFNGLDETACHSHADERPROC DetachShader;
extern PFNGLLINKPROGRAMPROC LinkProgram;
extern PFNGLUSEPROGRAMPROC UseProgram;
extern PFNGLVALIDATEPROGRAMPROC ValidateProgram;
extern PFNGLGETPROGRAMIVPROC GetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;

// Uniform functions
extern PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
extern PFNGLUNIFORM1FPROC Uniform1f;
extern PFNGLUNIFORM2FPROC Uniform2f;
extern PFNGLUNIFORM3FPROC Uniform3f;
extern PFNGLUNIFORM4FPROC Uniform4f;
extern PFNGLUNIFORM1IPROC Uniform1i;
extern PFNGLUNIFORM2IPROC Uniform2i;
extern PFNGLUNIFORM3IPROC Uniform3i;
extern PFNGLUNIFORM4IPROC Uniform4i;
extern PFNGLUNIFORM1FVPROC Uniform1fv;
extern PFNGLUNIFORM2FVPROC Uniform2fv;
extern PFNGLUNIFORM3FVPROC Uniform3fv;
extern PFNGLUNIFORM4FVPROC Uniform4fv;
extern PFNGLUNIFORM1IVPROC Uniform1iv;
extern PFNGLUNIFORM2IVPROC Uniform2iv;
extern PFNGLUNIFORM3IVPROC Uniform3iv;
extern PFNGLUNIFORM4IVPROC Uniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC UniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv;

// Uniform block functions
extern PFNGLGETUNIFORMBLOCKINDEXPROC GetUniformBlockIndex;
extern PFNGLUNIFORMBLOCKBINDINGPROC UniformBlockBinding;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC GetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC GetActiveUniformBlockName;

// Texture functions
extern PFNGLGENTEXTURESPROC GenTextures;
extern PFNGLDELETETEXTURESPROC DeleteTextures;
extern PFNGLBINDTEXTUREPROC BindTexture;
extern PFNGLACTIVETEXTUREPROC ActiveTexture;
extern PFNGLTEXIMAGE2DPROC TexImage2D;
extern PFNGLTEXIMAGE3DPROC TexImage3D;
extern PFNGLTEXSUBIMAGE2DPROC TexSubImage2D;
extern PFNGLTEXSUBIMAGE3DPROC TexSubImage3D;
extern PFNGLTEXPARAMETERIPROC TexParameteri;
extern PFNGLTEXPARAMETERFPROC TexParameterf;
extern PFNGLGENERATEMIPMAPPROC GenerateMipmap;
extern PFNGLPIXELSTOREIPROC PixelStorei;

// Sampler functions
extern PFNGLGENSAMPLERSPROC GenSamplers;
extern PFNGLDELETESAMPLERSPROC DeleteSamplers;
extern PFNGLBINDSAMPLERPROC BindSampler;
extern PFNGLSAMPLERPARAMETERIPROC SamplerParameteri;
extern PFNGLSAMPLERPARAMETERFPROC SamplerParameterf;

// Framebuffer functions
extern PFNGLGENFRAMEBUFFERSPROC GenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC DeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC BindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC FramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC FramebufferTextureLayer;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC FramebufferRenderbuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC CheckFramebufferStatus;
extern PFNGLBLITFRAMEBUFFERPROC BlitFramebuffer;

// Renderbuffer functions
extern PFNGLGENRENDERBUFFERSPROC GenRenderbuffers;
extern PFNGLDELETERENDERBUFFERSPROC DeleteRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC BindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC RenderbufferStorage;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC RenderbufferStorageMultisample;

// Sync functions
extern PFNGLFENCESYNCPROC FenceSync;
extern PFNGLDELETESYNCPROC DeleteSync;
extern PFNGLCLIENTWAITSYNCPROC ClientWaitSync;
extern PFNGLWAITSYNCPROC WaitSync;
extern PFNGLGETSYNCIVPROC GetSynciv;

// Query functions
extern PFNGLGENQUERIESPROC GenQueries;
extern PFNGLDELETEQUERIESPROC DeleteQueries;
extern PFNGLBEGINQUERYPROC BeginQuery;
extern PFNGLENDQUERYPROC EndQuery;
extern PFNGLGETQUERYIVPROC GetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC GetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC GetQueryObjectuiv;
extern PFNGLGETQUERYOBJECTI64VPROC GetQueryObjecti64v;
extern PFNGLGETQUERYOBJECTUI64VPROC GetQueryObjectui64v;
extern PFNGLQUERYCOUNTERPROC QueryCounter;

// Debug functions
extern PFNGLDEBUGMESSAGECALLBACKPROC DebugMessageCallback;
extern PFNGLDEBUGMESSAGECONTROLPROC DebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC DebugMessageInsert;
extern PFNGLGETDEBUGMESSAGELOGPROC GetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC PushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC PopDebugGroup;
extern PFNGLOBJECTLABELPROC ObjectLabel;
extern PFNGLGETOBJECTLABELPROC GetObjectLabel;
extern PFNGLOBJECTPTRLABELPROC ObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELPROC GetObjectPtrLabel;

// Draw functions
extern PFNGLDRAWARRAYSPROC DrawArrays;
extern PFNGLDRAWARRAYSINSTANCEDPROC DrawArraysInstanced;
extern PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC DrawArraysInstancedBaseInstance;
extern PFNGLDRAWELEMENTSPROC DrawElements;
extern PFNGLDRAWELEMENTSINSTANCEDPROC DrawElementsInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC DrawElementsInstancedBaseInstance;
extern PFNGLMULTIDRAWARRAYSPROC MultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC MultiDrawElements;
extern PFNGLMULTIDRAWARRAYSINDIRECTPROC MultiDrawArraysIndirect;
extern PFNGLMULTIDRAWELEMENTSINDIRECTPROC MultiDrawElementsIndirect;

// Compute shader functions
extern PFNGLDISPATCHCOMPUTEPROC DispatchCompute;
extern PFNGLDISPATCHCOMPUTEINDIRECTPROC DispatchComputeIndirect;
extern PFNGLMEMORYBARRIERPROC MemoryBarrier;

// Image/texture binding functions
extern PFNGLBINDIMAGETEXTUREPROC BindImageTexture;

// Capability and state functions
extern PFNGLENABLEPROC Enable;
extern PFNGLDISABLEPROC Disable;
extern PFNGLGETINTEGERVPROC GetIntegerv;
extern PFNGLGETFLOATVPROC GetFloatv;
extern PFNGLGETSTRINGPROC GetString;
extern PFNGLGETSTRINGIPROC GetStringi;
extern PFNGLVIEWPORTPROC Viewport;
extern PFNGLSCISSORPROC Scissor;
extern PFNGLCLEARCOLORPROC ClearColor;
extern PFNGLCLEARDEPTHPROC ClearDepth;
extern PFNGLCLEARPROC Clear;
extern PFNGLDEPTHFUNCPROC DepthFunc;
extern PFNGLDEPTHMASKPROC DepthMask;
extern PFNGLDEPTHRANGEPROC DepthRange;
extern PFNGLBLENDFUNCPROC BlendFunc;
extern PFNGLBLENDFUNCSEPARATEPROC BlendFuncSeparate;
extern PFNGLBLENDEQUATIONPROC BlendEquation;
extern PFNGLBLENDEQUATIONSEPARATEPROC BlendEquationSeparate;
extern PFNGLCULLFACEPROC CullFace;
extern PFNGLFRONTFACEPROC FrontFace;
extern PFNGLPOLYGONMODEPROC PolygonMode;
extern PFNGLPOLYGONOFFSETPROC PolygonOffset;
extern PFNGLSTENCILFUNCPROC StencilFunc;
extern PFNGLSTENCILMASKPROC StencilMask;
extern PFNGLSTENCILOPPROC StencilOp;

// DSA (Direct State Access) functions - OpenGL 4.5+
extern PFNGLCREATETEXTURESPROC CreateTextures;
extern PFNGLTEXTURESTORAGE2DPROC TextureStorage2D;
extern PFNGLTEXTURESUBIMAGE2DPROC TextureSubImage2D;
extern PFNGLBINDTEXTUREUNITPROC BindTextureUnit;
extern PFNGLTEXTUREPARAMETERIPROC TextureParameteri;
extern PFNGLGENERATETEXTUREMIPMAPPROC GenerateTextureMipmap;
extern PFNGLCREATEFRAMEBUFFERSPROC CreateFramebuffers;
extern PFNGLNAMEDFRAMEBUFFERTEXTUREPROC NamedFramebufferTexture;
extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC NamedFramebufferDrawBuffer;
extern PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC NamedFramebufferReadBuffer;
extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC CheckNamedFramebufferStatus;
extern PFNGLCREATEVERTEXARRAYSPROC CreateVertexArrays;
extern PFNGLENABLEVERTEXARRAYATTRIBPROC EnableVertexArrayAttrib;
extern PFNGLDISABLEVERTEXARRAYATTRIBPROC DisableVertexArrayAttrib;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC VertexArrayVertexBuffer;
extern PFNGLVERTEXARRAYATTRIBFORMATPROC VertexArrayAttribFormat;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC VertexArrayAttribBinding;

} // namespace GL
} // namespace nyaa

// =============================================================================
// Initialization Function
// =============================================================================

namespace nyaa {

// Initialize all modern OpenGL function pointers
// Call this ONCE after OpenGL context is created
void InitializeModernGLFunctions();

// Check if all critical OpenGL 4.3 functions were loaded successfully
bool IsModernGLSupported();

// Get OpenGL version string
const char* GetGLVersionString();

// Get OpenGL vendor string  
const char* GetGLVendorString();

// Get OpenGL renderer string
const char* GetGLRendererString();

} // namespace nyaa