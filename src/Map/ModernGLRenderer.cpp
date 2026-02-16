#include "ModernGLRenderer.h"
#include <cstdio>
#include <cstring>

// =============================================================================
// ModernGLRenderer - Phase 0 Implementation
// =============================================================================
// 
// This file implements the loading of all modern OpenGL 4.3+ function pointers.
// Functions are loaded using wglGetProcAddress on Windows.
//
// IMPORTANT: InitializeModernGLFunctions() must be called AFTER an OpenGL
// context has been created and made current.
//

namespace nyaa {
namespace GL {

// =============================================================================
// Function Pointer Definitions
// =============================================================================

// Vertex Array Object functions
PFNGLGENVERTEXARRAYSPROC GenVertexArrays = nullptr;
PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC DisableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC VertexAttribIPointer = nullptr;
PFNGLVERTEXATTRIBDIVISORPROC VertexAttribDivisor = nullptr;

// Buffer functions
PFNGLGENBUFFERSPROC GenBuffers = nullptr;
PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
PFNGLBINDBUFFERPROC BindBuffer = nullptr;
PFNGLBUFFERDATAPROC BufferData = nullptr;
PFNGLBUFFERSUBDATAPROC BufferSubData = nullptr;
PFNGLBUFFERSTORAGEPROC BufferStorage = nullptr;
PFNGLMAPBUFFERPROC MapBuffer = nullptr;
PFNGLMAPBUFFERRANGEPROC MapBufferRange = nullptr;
PFNGLUNMAPBUFFERPROC UnmapBuffer = nullptr;
PFNGLGETBUFFERSUBDATAPROC GetBufferSubData = nullptr;
PFNGLBINDBUFFERBASEPROC BindBufferBase = nullptr;
PFNGLBINDBUFFERRANGEPROC BindBufferRange = nullptr;

// DSA (Direct State Access) Buffer functions
PFNGLCREATEBUFFERSPROC CreateBuffers = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC NamedBufferStorage = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC NamedBufferSubData = nullptr;
PFNGLMAPNAMEDBUFFERRANGEPROC MapNamedBufferRange = nullptr;
PFNGLUNMAPNAMEDBUFFERPROC UnmapNamedBuffer = nullptr;
PFNGLGETNAMEDBUFFERSUBDATAPROC GetNamedBufferSubData = nullptr;

// Shader functions
PFNGLCREATESHADERPROC CreateShader = nullptr;
PFNGLDELETESHADERPROC DeleteShader = nullptr;
PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
PFNGLCOMPILESHADERPROC CompileShader = nullptr;
PFNGLGETSHADERIVPROC GetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog = nullptr;
PFNGLGETSHADERSOURCEPROC GetShaderSource = nullptr;

// Program functions
PFNGLCREATEPROGRAMPROC CreateProgram = nullptr;
PFNGLDELETEPROGRAMPROC DeleteProgram = nullptr;
PFNGLATTACHSHADERPROC AttachShader = nullptr;
PFNGLDETACHSHADERPROC DetachShader = nullptr;
PFNGLLINKPROGRAMPROC LinkProgram = nullptr;
PFNGLUSEPROGRAMPROC UseProgram = nullptr;
PFNGLVALIDATEPROGRAMPROC ValidateProgram = nullptr;
PFNGLGETPROGRAMIVPROC GetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog = nullptr;

// Uniform functions
PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation = nullptr;
PFNGLUNIFORM1FPROC Uniform1f = nullptr;
PFNGLUNIFORM2FPROC Uniform2f = nullptr;
PFNGLUNIFORM3FPROC Uniform3f = nullptr;
PFNGLUNIFORM4FPROC Uniform4f = nullptr;
PFNGLUNIFORM1IPROC Uniform1i = nullptr;
PFNGLUNIFORM2IPROC Uniform2i = nullptr;
PFNGLUNIFORM3IPROC Uniform3i = nullptr;
PFNGLUNIFORM4IPROC Uniform4i = nullptr;
PFNGLUNIFORM1FVPROC Uniform1fv = nullptr;
PFNGLUNIFORM2FVPROC Uniform2fv = nullptr;
PFNGLUNIFORM3FVPROC Uniform3fv = nullptr;
PFNGLUNIFORM4FVPROC Uniform4fv = nullptr;
PFNGLUNIFORM1IVPROC Uniform1iv = nullptr;
PFNGLUNIFORM2IVPROC Uniform2iv = nullptr;
PFNGLUNIFORM3IVPROC Uniform3iv = nullptr;
PFNGLUNIFORM4IVPROC Uniform4iv = nullptr;
PFNGLUNIFORMMATRIX2FVPROC UniformMatrix2fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv = nullptr;

// Uniform block functions
PFNGLGETUNIFORMBLOCKINDEXPROC GetUniformBlockIndex = nullptr;
PFNGLUNIFORMBLOCKBINDINGPROC UniformBlockBinding = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC GetActiveUniformBlockiv = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC GetActiveUniformBlockName = nullptr;

// Texture functions
PFNGLGENTEXTURESPROC GenTextures = nullptr;
PFNGLDELETETEXTURESPROC DeleteTextures = nullptr;
PFNGLBINDTEXTUREPROC BindTexture = nullptr;
PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;
PFNGLTEXIMAGE2DPROC TexImage2D = nullptr;
PFNGLTEXIMAGE3DPROC TexImage3D = nullptr;
PFNGLTEXSUBIMAGE2DPROC TexSubImage2D = nullptr;
PFNGLTEXSUBIMAGE3DPROC TexSubImage3D = nullptr;
PFNGLTEXPARAMETERIPROC TexParameteri = nullptr;
PFNGLTEXPARAMETERFPROC TexParameterf = nullptr;
PFNGLGENERATEMIPMAPPROC GenerateMipmap = nullptr;
PFNGLPIXELSTOREIPROC PixelStorei = nullptr;

// Sampler functions
PFNGLGENSAMPLERSPROC GenSamplers = nullptr;
PFNGLDELETESAMPLERSPROC DeleteSamplers = nullptr;
PFNGLBINDSAMPLERPROC BindSampler = nullptr;
PFNGLSAMPLERPARAMETERIPROC SamplerParameteri = nullptr;
PFNGLSAMPLERPARAMETERFPROC SamplerParameterf = nullptr;

// Framebuffer functions
PFNGLGENFRAMEBUFFERSPROC GenFramebuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC DeleteFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC BindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC FramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC FramebufferTextureLayer = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC FramebufferRenderbuffer = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC CheckFramebufferStatus = nullptr;
PFNGLBLITFRAMEBUFFERPROC BlitFramebuffer = nullptr;

// Renderbuffer functions
PFNGLGENRENDERBUFFERSPROC GenRenderbuffers = nullptr;
PFNGLDELETERENDERBUFFERSPROC DeleteRenderbuffers = nullptr;
PFNGLBINDRENDERBUFFERPROC BindRenderbuffer = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC RenderbufferStorage = nullptr;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC RenderbufferStorageMultisample = nullptr;

// Sync functions
PFNGLFENCESYNCPROC FenceSync = nullptr;
PFNGLDELETESYNCPROC DeleteSync = nullptr;
PFNGLCLIENTWAITSYNCPROC ClientWaitSync = nullptr;
PFNGLWAITSYNCPROC WaitSync = nullptr;
PFNGLGETSYNCIVPROC GetSynciv = nullptr;

// Query functions
PFNGLGENQUERIESPROC GenQueries = nullptr;
PFNGLDELETEQUERIESPROC DeleteQueries = nullptr;
PFNGLBEGINQUERYPROC BeginQuery = nullptr;
PFNGLENDQUERYPROC EndQuery = nullptr;
PFNGLGETQUERYIVPROC GetQueryiv = nullptr;
PFNGLGETQUERYOBJECTIVPROC GetQueryObjectiv = nullptr;
PFNGLGETQUERYOBJECTUIVPROC GetQueryObjectuiv = nullptr;
PFNGLGETQUERYOBJECTI64VPROC GetQueryObjecti64v = nullptr;
PFNGLGETQUERYOBJECTUI64VPROC GetQueryObjectui64v = nullptr;
PFNGLQUERYCOUNTERPROC QueryCounter = nullptr;

// Debug functions
PFNGLDEBUGMESSAGECALLBACKPROC DebugMessageCallback = nullptr;
PFNGLDEBUGMESSAGECONTROLPROC DebugMessageControl = nullptr;
PFNGLDEBUGMESSAGEINSERTPROC DebugMessageInsert = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC GetDebugMessageLog = nullptr;
PFNGLPUSHDEBUGGROUPPROC PushDebugGroup = nullptr;
PFNGLPOPDEBUGGROUPPROC PopDebugGroup = nullptr;
PFNGLOBJECTLABELPROC ObjectLabel = nullptr;
PFNGLGETOBJECTLABELPROC GetObjectLabel = nullptr;
PFNGLOBJECTPTRLABELPROC ObjectPtrLabel = nullptr;
PFNGLGETOBJECTPTRLABELPROC GetObjectPtrLabel = nullptr;

// Draw functions
PFNGLDRAWARRAYSPROC DrawArrays = nullptr;
PFNGLDRAWARRAYSINSTANCEDPROC DrawArraysInstanced = nullptr;
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC DrawArraysInstancedBaseInstance = nullptr;
PFNGLDRAWELEMENTSPROC DrawElements = nullptr;
PFNGLDRAWELEMENTSINSTANCEDPROC DrawElementsInstanced = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC DrawElementsInstancedBaseInstance = nullptr;
PFNGLMULTIDRAWARRAYSPROC MultiDrawArrays = nullptr;
PFNGLMULTIDRAWELEMENTSPROC MultiDrawElements = nullptr;
PFNGLMULTIDRAWARRAYSINDIRECTPROC MultiDrawArraysIndirect = nullptr;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC MultiDrawElementsIndirect = nullptr;

// Compute shader functions
PFNGLDISPATCHCOMPUTEPROC DispatchCompute = nullptr;
PFNGLDISPATCHCOMPUTEINDIRECTPROC DispatchComputeIndirect = nullptr;
PFNGLMEMORYBARRIERPROC MemoryBarrier = nullptr;

// Image/texture binding functions
PFNGLBINDIMAGETEXTUREPROC BindImageTexture = nullptr;

// Capability and state functions
PFNGLENABLEPROC Enable = nullptr;
PFNGLDISABLEPROC Disable = nullptr;
PFNGLGETINTEGERVPROC GetIntegerv = nullptr;
PFNGLGETFLOATVPROC GetFloatv = nullptr;
PFNGLGETSTRINGPROC GetString = nullptr;
PFNGLGETSTRINGIPROC GetStringi = nullptr;
PFNGLVIEWPORTPROC Viewport = nullptr;
PFNGLSCISSORPROC Scissor = nullptr;
PFNGLCLEARCOLORPROC ClearColor = nullptr;
PFNGLCLEARDEPTHPROC ClearDepth = nullptr;
PFNGLCLEARPROC Clear = nullptr;
PFNGLDEPTHFUNCPROC DepthFunc = nullptr;
PFNGLDEPTHMASKPROC DepthMask = nullptr;
PFNGLDEPTHRANGEPROC DepthRange = nullptr;
PFNGLBLENDFUNCPROC BlendFunc = nullptr;
PFNGLBLENDFUNCSEPARATEPROC BlendFuncSeparate = nullptr;
PFNGLBLENDEQUATIONPROC BlendEquation = nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC BlendEquationSeparate = nullptr;
PFNGLCULLFACEPROC CullFace = nullptr;
PFNGLFRONTFACEPROC FrontFace = nullptr;
PFNGLPOLYGONMODEPROC PolygonMode = nullptr;
PFNGLPOLYGONOFFSETPROC PolygonOffset = nullptr;
PFNGLSTENCILFUNCPROC StencilFunc = nullptr;
PFNGLSTENCILMASKPROC StencilMask = nullptr;
PFNGLSTENCILOPPROC StencilOp = nullptr;

// DSA (Direct State Access) functions - OpenGL 4.5+
PFNGLCREATETEXTURESPROC CreateTextures = nullptr;
PFNGLTEXTURESTORAGE2DPROC TextureStorage2D = nullptr;
PFNGLTEXTURESUBIMAGE2DPROC TextureSubImage2D = nullptr;
PFNGLBINDTEXTUREUNITPROC BindTextureUnit = nullptr;
PFNGLTEXTUREPARAMETERIPROC TextureParameteri = nullptr;
PFNGLGENERATETEXTUREMIPMAPPROC GenerateTextureMipmap = nullptr;
PFNGLCREATEFRAMEBUFFERSPROC CreateFramebuffers = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC NamedFramebufferTexture = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC NamedFramebufferDrawBuffer = nullptr;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC NamedFramebufferReadBuffer = nullptr;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC CheckNamedFramebufferStatus = nullptr;
PFNGLCREATEVERTEXARRAYSPROC CreateVertexArrays = nullptr;
PFNGLENABLEVERTEXARRAYATTRIBPROC EnableVertexArrayAttrib = nullptr;
PFNGLDISABLEVERTEXARRAYATTRIBPROC DisableVertexArrayAttrib = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERPROC VertexArrayVertexBuffer = nullptr;
PFNGLVERTEXARRAYATTRIBFORMATPROC VertexArrayAttribFormat = nullptr;
PFNGLVERTEXARRAYATTRIBBINDINGPROC VertexArrayAttribBinding = nullptr;

} // namespace GL

// =============================================================================
// Helper Macros for Loading Function Pointers
// =============================================================================

#ifdef _WIN32
    #define GL_GET_PROC_ADDRESS(name) reinterpret_cast<void*>(wglGetProcAddress(name))
#else
    // For non-Windows platforms, would need platform-specific code
    #define GL_GET_PROC_ADDRESS(name) nullptr
#endif

#define LOAD_GL_FUNCTION(func, name) \
    func = reinterpret_cast<decltype(func)>(GL_GET_PROC_ADDRESS(name))

#define LOAD_GL_FUNCTION_OPTIONAL(func, name) \
    func = reinterpret_cast<decltype(func)>(GL_GET_PROC_ADDRESS(name))

// =============================================================================
// Initialization Function Implementation
// =============================================================================

static bool s_initialized = false;

void InitializeModernGLFunctions()
{
    if (s_initialized) {
        return; // Already initialized
    }
    
    s_initialized = true;
    
    // =============================================================================
    // Load Vertex Array Object functions (OpenGL 3.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenVertexArrays, "glGenVertexArrays");
    LOAD_GL_FUNCTION(GL::DeleteVertexArrays, "glDeleteVertexArrays");
    LOAD_GL_FUNCTION(GL::BindVertexArray, "glBindVertexArray");
    LOAD_GL_FUNCTION(GL::EnableVertexAttribArray, "glEnableVertexAttribArray");
    LOAD_GL_FUNCTION(GL::DisableVertexAttribArray, "glDisableVertexAttribArray");
    LOAD_GL_FUNCTION(GL::VertexAttribPointer, "glVertexAttribPointer");
    LOAD_GL_FUNCTION(GL::VertexAttribIPointer, "glVertexAttribIPointer");
    LOAD_GL_FUNCTION(GL::VertexAttribDivisor, "glVertexAttribDivisor");
    
    // =============================================================================
    // Load Buffer functions (OpenGL 1.5+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenBuffers, "glGenBuffers");
    LOAD_GL_FUNCTION(GL::DeleteBuffers, "glDeleteBuffers");
    LOAD_GL_FUNCTION(GL::BindBuffer, "glBindBuffer");
    LOAD_GL_FUNCTION(GL::BufferData, "glBufferData");
    LOAD_GL_FUNCTION(GL::BufferSubData, "glBufferSubData");
    LOAD_GL_FUNCTION(GL::MapBuffer, "glMapBuffer");
    LOAD_GL_FUNCTION(GL::UnmapBuffer, "glUnmapBuffer");
    LOAD_GL_FUNCTION(GL::GetBufferSubData, "glGetBufferSubData");
    LOAD_GL_FUNCTION(GL::BindBufferBase, "glBindBufferBase");
    LOAD_GL_FUNCTION(GL::BindBufferRange, "glBindBufferRange");
    
    // =============================================================================
    // Load Buffer Storage functions (OpenGL 4.4+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::BufferStorage, "glBufferStorage");
    LOAD_GL_FUNCTION(GL::MapBufferRange, "glMapBufferRange");
    
    // =============================================================================
    // Load DSA Buffer functions (OpenGL 4.5+) - Optional
    // =============================================================================
    LOAD_GL_FUNCTION_OPTIONAL(GL::CreateBuffers, "glCreateBuffers");
    LOAD_GL_FUNCTION_OPTIONAL(GL::NamedBufferStorage, "glNamedBufferStorage");
    LOAD_GL_FUNCTION_OPTIONAL(GL::NamedBufferSubData, "glNamedBufferSubData");
    LOAD_GL_FUNCTION_OPTIONAL(GL::MapNamedBufferRange, "glMapNamedBufferRange");
    LOAD_GL_FUNCTION_OPTIONAL(GL::UnmapNamedBuffer, "glUnmapNamedBuffer");
    LOAD_GL_FUNCTION_OPTIONAL(GL::GetNamedBufferSubData, "glGetNamedBufferSubData");
    
    // =============================================================================
    // Load Shader functions (OpenGL 2.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::CreateShader, "glCreateShader");
    LOAD_GL_FUNCTION(GL::DeleteShader, "glDeleteShader");
    LOAD_GL_FUNCTION(GL::ShaderSource, "glShaderSource");
    LOAD_GL_FUNCTION(GL::CompileShader, "glCompileShader");
    LOAD_GL_FUNCTION(GL::GetShaderiv, "glGetShaderiv");
    LOAD_GL_FUNCTION(GL::GetShaderInfoLog, "glGetShaderInfoLog");
    LOAD_GL_FUNCTION(GL::GetShaderSource, "glGetShaderSource");
    
    // =============================================================================
    // Load Program functions (OpenGL 2.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::CreateProgram, "glCreateProgram");
    LOAD_GL_FUNCTION(GL::DeleteProgram, "glDeleteProgram");
    LOAD_GL_FUNCTION(GL::AttachShader, "glAttachShader");
    LOAD_GL_FUNCTION(GL::DetachShader, "glDetachShader");
    LOAD_GL_FUNCTION(GL::LinkProgram, "glLinkProgram");
    LOAD_GL_FUNCTION(GL::UseProgram, "glUseProgram");
    LOAD_GL_FUNCTION(GL::ValidateProgram, "glValidateProgram");
    LOAD_GL_FUNCTION(GL::GetProgramiv, "glGetProgramiv");
    LOAD_GL_FUNCTION(GL::GetProgramInfoLog, "glGetProgramInfoLog");
    
    // =============================================================================
    // Load Uniform functions (OpenGL 2.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GetUniformLocation, "glGetUniformLocation");
    LOAD_GL_FUNCTION(GL::Uniform1f, "glUniform1f");
    LOAD_GL_FUNCTION(GL::Uniform2f, "glUniform2f");
    LOAD_GL_FUNCTION(GL::Uniform3f, "glUniform3f");
    LOAD_GL_FUNCTION(GL::Uniform4f, "glUniform4f");
    LOAD_GL_FUNCTION(GL::Uniform1i, "glUniform1i");
    LOAD_GL_FUNCTION(GL::Uniform2i, "glUniform2i");
    LOAD_GL_FUNCTION(GL::Uniform3i, "glUniform3i");
    LOAD_GL_FUNCTION(GL::Uniform4i, "glUniform4i");
    LOAD_GL_FUNCTION(GL::Uniform1fv, "glUniform1fv");
    LOAD_GL_FUNCTION(GL::Uniform2fv, "glUniform2fv");
    LOAD_GL_FUNCTION(GL::Uniform3fv, "glUniform3fv");
    LOAD_GL_FUNCTION(GL::Uniform4fv, "glUniform4fv");
    LOAD_GL_FUNCTION(GL::Uniform1iv, "glUniform1iv");
    LOAD_GL_FUNCTION(GL::Uniform2iv, "glUniform2iv");
    LOAD_GL_FUNCTION(GL::Uniform3iv, "glUniform3iv");
    LOAD_GL_FUNCTION(GL::Uniform4iv, "glUniform4iv");
    LOAD_GL_FUNCTION(GL::UniformMatrix2fv, "glUniformMatrix2fv");
    LOAD_GL_FUNCTION(GL::UniformMatrix3fv, "glUniformMatrix3fv");
    LOAD_GL_FUNCTION(GL::UniformMatrix4fv, "glUniformMatrix4fv");
    
    // =============================================================================
    // Load Uniform block functions (OpenGL 3.1+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GetUniformBlockIndex, "glGetUniformBlockIndex");
    LOAD_GL_FUNCTION(GL::UniformBlockBinding, "glUniformBlockBinding");
    LOAD_GL_FUNCTION(GL::GetActiveUniformBlockiv, "glGetActiveUniformBlockiv");
    LOAD_GL_FUNCTION(GL::GetActiveUniformBlockName, "glGetActiveUniformBlockName");
    
    // =============================================================================
    // Load Texture functions (OpenGL 1.1+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenTextures, "glGenTextures");
    LOAD_GL_FUNCTION(GL::DeleteTextures, "glDeleteTextures");
    LOAD_GL_FUNCTION(GL::BindTexture, "glBindTexture");
    LOAD_GL_FUNCTION(GL::ActiveTexture, "glActiveTexture");
    LOAD_GL_FUNCTION(GL::TexImage2D, "glTexImage2D");
    LOAD_GL_FUNCTION(GL::TexImage3D, "glTexImage3D");
    LOAD_GL_FUNCTION(GL::TexSubImage2D, "glTexSubImage2D");
    LOAD_GL_FUNCTION(GL::TexSubImage3D, "glTexSubImage3D");
    LOAD_GL_FUNCTION(GL::TexParameteri, "glTexParameteri");
    LOAD_GL_FUNCTION(GL::TexParameterf, "glTexParameterf");
    LOAD_GL_FUNCTION(GL::GenerateMipmap, "glGenerateMipmap");
    LOAD_GL_FUNCTION(GL::PixelStorei, "glPixelStorei");
    
    // =============================================================================
    // Load Sampler functions (OpenGL 3.3+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenSamplers, "glGenSamplers");
    LOAD_GL_FUNCTION(GL::DeleteSamplers, "glDeleteSamplers");
    LOAD_GL_FUNCTION(GL::BindSampler, "glBindSampler");
    LOAD_GL_FUNCTION(GL::SamplerParameteri, "glSamplerParameteri");
    LOAD_GL_FUNCTION(GL::SamplerParameterf, "glSamplerParameterf");
    
    // =============================================================================
    // Load Framebuffer functions (OpenGL 3.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenFramebuffers, "glGenFramebuffers");
    LOAD_GL_FUNCTION(GL::DeleteFramebuffers, "glDeleteFramebuffers");
    LOAD_GL_FUNCTION(GL::BindFramebuffer, "glBindFramebuffer");
    LOAD_GL_FUNCTION(GL::FramebufferTexture2D, "glFramebufferTexture2D");
    LOAD_GL_FUNCTION(GL::FramebufferTextureLayer, "glFramebufferTextureLayer");
    LOAD_GL_FUNCTION(GL::FramebufferRenderbuffer, "glFramebufferRenderbuffer");
    LOAD_GL_FUNCTION(GL::CheckFramebufferStatus, "glCheckFramebufferStatus");
    LOAD_GL_FUNCTION(GL::BlitFramebuffer, "glBlitFramebuffer");
    
    // =============================================================================
    // Load Renderbuffer functions (OpenGL 3.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenRenderbuffers, "glGenRenderbuffers");
    LOAD_GL_FUNCTION(GL::DeleteRenderbuffers, "glDeleteRenderbuffers");
    LOAD_GL_FUNCTION(GL::BindRenderbuffer, "glBindRenderbuffer");
    LOAD_GL_FUNCTION(GL::RenderbufferStorage, "glRenderbufferStorage");
    LOAD_GL_FUNCTION(GL::RenderbufferStorageMultisample, "glRenderbufferStorageMultisample");
    
    // =============================================================================
    // Load Sync functions (OpenGL 3.2+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::FenceSync, "glFenceSync");
    LOAD_GL_FUNCTION(GL::DeleteSync, "glDeleteSync");
    LOAD_GL_FUNCTION(GL::ClientWaitSync, "glClientWaitSync");
    LOAD_GL_FUNCTION(GL::WaitSync, "glWaitSync");
    LOAD_GL_FUNCTION(GL::GetSynciv, "glGetSynciv");
    
    // =============================================================================
    // Load Query functions (OpenGL 1.5+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::GenQueries, "glGenQueries");
    LOAD_GL_FUNCTION(GL::DeleteQueries, "glDeleteQueries");
    LOAD_GL_FUNCTION(GL::BeginQuery, "glBeginQuery");
    LOAD_GL_FUNCTION(GL::EndQuery, "glEndQuery");
    LOAD_GL_FUNCTION(GL::GetQueryiv, "glGetQueryiv");
    LOAD_GL_FUNCTION(GL::GetQueryObjectiv, "glGetQueryObjectiv");
    LOAD_GL_FUNCTION(GL::GetQueryObjectuiv, "glGetQueryObjectuiv");
    LOAD_GL_FUNCTION(GL::GetQueryObjecti64v, "glGetQueryObjecti64v");
    LOAD_GL_FUNCTION(GL::GetQueryObjectui64v, "glGetQueryObjectui64v");
    LOAD_GL_FUNCTION(GL::QueryCounter, "glQueryCounter");
    
    // =============================================================================
    // Load Debug functions (OpenGL 4.3+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::DebugMessageCallback, "glDebugMessageCallback");
    LOAD_GL_FUNCTION(GL::DebugMessageControl, "glDebugMessageControl");
    LOAD_GL_FUNCTION(GL::DebugMessageInsert, "glDebugMessageInsert");
    LOAD_GL_FUNCTION(GL::GetDebugMessageLog, "glGetDebugMessageLog");
    LOAD_GL_FUNCTION(GL::PushDebugGroup, "glPushDebugGroup");
    LOAD_GL_FUNCTION(GL::PopDebugGroup, "glPopDebugGroup");
    LOAD_GL_FUNCTION(GL::ObjectLabel, "glObjectLabel");
    LOAD_GL_FUNCTION(GL::GetObjectLabel, "glGetObjectLabel");
    LOAD_GL_FUNCTION(GL::ObjectPtrLabel, "glObjectPtrLabel");
    LOAD_GL_FUNCTION(GL::GetObjectPtrLabel, "glGetObjectPtrLabel");
    
    // =============================================================================
    // Load Draw functions (OpenGL 1.1+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::DrawArrays, "glDrawArrays");
    LOAD_GL_FUNCTION(GL::DrawArraysInstanced, "glDrawArraysInstanced");
    LOAD_GL_FUNCTION(GL::DrawArraysInstancedBaseInstance, "glDrawArraysInstancedBaseInstance");
    LOAD_GL_FUNCTION(GL::DrawElements, "glDrawElements");
    LOAD_GL_FUNCTION(GL::DrawElementsInstanced, "glDrawElementsInstanced");
    LOAD_GL_FUNCTION(GL::DrawElementsInstancedBaseInstance, "glDrawElementsInstancedBaseInstance");
    LOAD_GL_FUNCTION(GL::MultiDrawArrays, "glMultiDrawArrays");
    LOAD_GL_FUNCTION(GL::MultiDrawElements, "glMultiDrawElements");
    LOAD_GL_FUNCTION(GL::MultiDrawArraysIndirect, "glMultiDrawArraysIndirect");
    LOAD_GL_FUNCTION(GL::MultiDrawElementsIndirect, "glMultiDrawElementsIndirect");
    
    // =============================================================================
    // Load Compute shader functions (OpenGL 4.3+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::DispatchCompute, "glDispatchCompute");
    LOAD_GL_FUNCTION(GL::DispatchComputeIndirect, "glDispatchComputeIndirect");
    LOAD_GL_FUNCTION(GL::MemoryBarrier, "glMemoryBarrier");
    
    // =============================================================================
    // Load Image/texture binding functions (OpenGL 4.2+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::BindImageTexture, "glBindImageTexture");
    
    // =============================================================================
    // Load Capability and state functions (OpenGL 1.0+)
    // =============================================================================
    LOAD_GL_FUNCTION(GL::Enable, "glEnable");
    LOAD_GL_FUNCTION(GL::Disable, "glDisable");
    LOAD_GL_FUNCTION(GL::GetIntegerv, "glGetIntegerv");
    LOAD_GL_FUNCTION(GL::GetFloatv, "glGetFloatv");
    LOAD_GL_FUNCTION(GL::GetString, "glGetString");
    LOAD_GL_FUNCTION(GL::GetStringi, "glGetStringi");
    LOAD_GL_FUNCTION(GL::Viewport, "glViewport");
    LOAD_GL_FUNCTION(GL::Scissor, "glScissor");
    LOAD_GL_FUNCTION(GL::ClearColor, "glClearColor");
    LOAD_GL_FUNCTION(GL::ClearDepth, "glClearDepth");
    LOAD_GL_FUNCTION(GL::Clear, "glClear");
    LOAD_GL_FUNCTION(GL::DepthFunc, "glDepthFunc");
    LOAD_GL_FUNCTION(GL::DepthMask, "glDepthMask");
    LOAD_GL_FUNCTION(GL::DepthRange, "glDepthRange");
    LOAD_GL_FUNCTION(GL::BlendFunc, "glBlendFunc");
    LOAD_GL_FUNCTION(GL::BlendFuncSeparate, "glBlendFuncSeparate");
    LOAD_GL_FUNCTION(GL::BlendEquation, "glBlendEquation");
    LOAD_GL_FUNCTION(GL::BlendEquationSeparate, "glBlendEquationSeparate");
    LOAD_GL_FUNCTION(GL::CullFace, "glCullFace");
    LOAD_GL_FUNCTION(GL::FrontFace, "glFrontFace");
    LOAD_GL_FUNCTION(GL::PolygonMode, "glPolygonMode");
    LOAD_GL_FUNCTION(GL::PolygonOffset, "glPolygonOffset");
    LOAD_GL_FUNCTION(GL::StencilFunc, "glStencilFunc");
    LOAD_GL_FUNCTION(GL::StencilMask, "glStencilMask");
    LOAD_GL_FUNCTION(GL::StencilOp, "glStencilOp");
    
    // =============================================================================
    // Load DSA functions (OpenGL 4.5+) - Optional
    // =============================================================================
    LOAD_GL_FUNCTION_OPTIONAL(GL::CreateTextures, "glCreateTextures");
    LOAD_GL_FUNCTION_OPTIONAL(GL::TextureStorage2D, "glTextureStorage2D");
    LOAD_GL_FUNCTION_OPTIONAL(GL::TextureSubImage2D, "glTextureSubImage2D");
    LOAD_GL_FUNCTION_OPTIONAL(GL::BindTextureUnit, "glBindTextureUnit");
    LOAD_GL_FUNCTION_OPTIONAL(GL::TextureParameteri, "glTextureParameteri");
    LOAD_GL_FUNCTION_OPTIONAL(GL::GenerateTextureMipmap, "glGenerateTextureMipmap");
    LOAD_GL_FUNCTION_OPTIONAL(GL::CreateFramebuffers, "glCreateFramebuffers");
    LOAD_GL_FUNCTION_OPTIONAL(GL::NamedFramebufferTexture, "glNamedFramebufferTexture");
    LOAD_GL_FUNCTION_OPTIONAL(GL::NamedFramebufferDrawBuffer, "glNamedFramebufferDrawBuffer");
    LOAD_GL_FUNCTION_OPTIONAL(GL::NamedFramebufferReadBuffer, "glNamedFramebufferReadBuffer");
    LOAD_GL_FUNCTION_OPTIONAL(GL::CheckNamedFramebufferStatus, "glCheckNamedFramebufferStatus");
    LOAD_GL_FUNCTION_OPTIONAL(GL::CreateVertexArrays, "glCreateVertexArrays");
    LOAD_GL_FUNCTION_OPTIONAL(GL::EnableVertexArrayAttrib, "glEnableVertexArrayAttrib");
    LOAD_GL_FUNCTION_OPTIONAL(GL::DisableVertexArrayAttrib, "glDisableVertexArrayAttrib");
    LOAD_GL_FUNCTION_OPTIONAL(GL::VertexArrayVertexBuffer, "glVertexArrayVertexBuffer");
    LOAD_GL_FUNCTION_OPTIONAL(GL::VertexArrayAttribFormat, "glVertexArrayAttribFormat");
    LOAD_GL_FUNCTION_OPTIONAL(GL::VertexArrayAttribBinding, "glVertexArrayAttribBinding");
}

// =============================================================================
// Support Check Functions
// =============================================================================

bool IsModernGLSupported()
{
    // Check if critical OpenGL 4.3 functions were loaded
    if (GL::GenVertexArrays == nullptr) return false;
    if (GL::BindVertexArray == nullptr) return false;
    if (GL::GenBuffers == nullptr) return false;
    if (GL::BindBuffer == nullptr) return false;
    if (GL::BufferStorage == nullptr) return false;
    if (GL::CreateShader == nullptr) return false;
    if (GL::CreateProgram == nullptr) return false;
    if (GL::UseProgram == nullptr) return false;
    if (GL::DispatchCompute == nullptr) return false;
    if (GL::FenceSync == nullptr) return false;
    if (GL::DebugMessageCallback == nullptr) return false;
    
    return true;
}

const char* GetGLVersionString()
{
    if (GL::GetString != nullptr) {
        return reinterpret_cast<const char*>(GL::GetString(0x1F02)); // GL_VERSION
    }
    return "Unknown";
}

const char* GetGLVendorString()
{
    if (GL::GetString != nullptr) {
        return reinterpret_cast<const char*>(GL::GetString(0x1F00)); // GL_VENDOR
    }
    return "Unknown";
}

const char* GetGLRendererString()
{
    if (GL::GetString != nullptr) {
        return reinterpret_cast<const char*>(GL::GetString(0x1F01)); // GL_RENDERER
    }
    return "Unknown";
}

} // namespace nyaa