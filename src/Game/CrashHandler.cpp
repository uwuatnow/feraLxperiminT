#include "CrashHandler.h"
#include <iostream>
#include <csignal>
#include <exception>
#include <cstdlib>
#include <string>

#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
#include <stacktrace>
#define HAS_STACKTRACE 1
#else
#define HAS_STACKTRACE 0
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <sstream>
#include <iomanip>
#endif

namespace nyaa {

std::vector<std::function<void()>> CrashHandler::onCrashHandlers;

void CrashHandler::install() {
    setupSignalHandlers();
    setupUnhandledExceptionHandler();
    
#ifdef _WIN32
    setupWindowsUnhandledExceptionFilter();
#endif
}

void CrashHandler::setupSignalHandlers() {
    // Install signal handlers for common crash signals
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
}

void CrashHandler::setupUnhandledExceptionHandler() {
    // Set custom terminate handler for uncaught exceptions
    std::set_terminate(unhandledExceptionTerminateHandler);
}

void CrashHandler::signalHandler(int sig) {
    std::cerr << "\n=== CRASH DETECTED ===\n";
    std::cerr << "Signal: " << sig << " (";
    
    switch(sig) {
        case SIGSEGV: std::cerr << "Segmentation fault"; break;
        case SIGABRT: std::cerr << "Abort signal"; break;
        case SIGFPE: std::cerr << "Floating point exception"; break;
        case SIGILL: std::cerr << "Illegal instruction"; break;
        case SIGTERM: std::cerr << "Termination signal"; break;
        case SIGINT: std::cerr << "Interrupt signal"; break;
        default: std::cerr << "Unknown signal"; break;
    }
    std::cerr << ")\n";
    
    printStacktrace();
    
    CallCrashHandlers();
    
    std::cerr << "=== CRASH HANDLER EXITING ===\n";
    
    // Restore default signal handler and re-raise
    ::signal(sig, SIG_DFL);
    raise(sig);
}

void CrashHandler::unhandledExceptionTerminateHandler() {
    std::cerr << "\n=== UNCAUGHT EXCEPTION DETECTED ===\n";
    
    try {
        // Re-throw the current exception
        throw;
    } catch (const std::exception& e) {
        printExceptionStacktrace(e);
    } catch (...) {
        std::cerr << "Unknown exception type\n";
        printStacktrace();
    }
    
    CallCrashHandlers();
    
    std::cerr << "=== TERMINATE HANDLER EXITING ===\n";
    std::abort();
}

void CrashHandler::printStacktrace() {
#if HAS_STACKTRACE
    try {
        auto trace = std::stacktrace::current();
        std::cerr << "Stack trace:\n" << trace << "\n";
    } catch (...) {
        std::cerr << "Failed to capture stacktrace\n";
    }
#else
    #ifdef _WIN32
        printWindowsStacktrace();
    #else
        std::cerr << "Stacktrace not available (C++23 std::stacktrace not supported)\n";
        std::cerr << "Compile with -std=c++23 and a compatible compiler (GCC 13+, Clang 16+)\n";
    #endif
#endif
}

void CrashHandler::printExceptionStacktrace(const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    printStacktrace();
}

void CrashHandler::printCurrentStacktrace(const std::string& message) {
    if (!message.empty()) {
        std::cerr << message << "\n";
    }
    printStacktrace();
}

#ifdef _WIN32

void CrashHandler::setupWindowsUnhandledExceptionFilter() {
    SetUnhandledExceptionFilter(windowsUnhandledExceptionFilter);
}

long __stdcall CrashHandler::windowsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo) {
    std::cerr << "\n=== WINDOWS UNHANDLED EXCEPTION DETECTED ===\n";
    
    DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;
    std::cerr << "Exception Code: 0x" << std::hex << exceptionCode << std::dec << " (";
    
    switch(exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            std::cerr << "Access violation";
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            std::cerr << "Array bounds exceeded";
            break;
        case EXCEPTION_BREAKPOINT:
            std::cerr << "Breakpoint";
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            std::cerr << "Datatype misalignment";
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            std::cerr << "Floating point denormal operand";
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            std::cerr << "Floating point divide by zero";
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            std::cerr << "Floating point inexact result";
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            std::cerr << "Floating point invalid operation";
            break;
        case EXCEPTION_FLT_OVERFLOW:
            std::cerr << "Floating point overflow";
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            std::cerr << "Floating point stack check";
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            std::cerr << "Floating point underflow";
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            std::cerr << "Illegal instruction";
            break;
        case EXCEPTION_IN_PAGE_ERROR:
            std::cerr << "In-page error";
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            std::cerr << "Integer divide by zero";
            break;
        case EXCEPTION_INT_OVERFLOW:
            std::cerr << "Integer overflow";
            break;
        case EXCEPTION_INVALID_DISPOSITION:
            std::cerr << "Invalid disposition";
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            std::cerr << "Noncontinuable exception";
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            std::cerr << "Privileged instruction";
            break;
        case EXCEPTION_SINGLE_STEP:
            std::cerr << "Single step";
            break;
        case EXCEPTION_STACK_OVERFLOW:
            std::cerr << "Stack overflow";
            break;
        default:
            std::cerr << "Unknown exception";
            break;
    }
    std::cerr << ")\n";
    
    printWindowsStacktrace(exceptionInfo);
    std::cerr << "=== WINDOWS EXCEPTION HANDLER EXITING ===\n";
    
    CallCrashHandlers();
    
    // Return EXCEPTION_EXECUTE_HANDLER to prevent the system from handling it
    return EXCEPTION_EXECUTE_HANDLER;
}

void CrashHandler::printWindowsStacktrace(struct _EXCEPTION_POINTERS* exceptionInfo) {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    // Initialize symbol handler
    SymInitialize(process, NULL, TRUE);
    
    // Get context
    CONTEXT context;
    memcpy(&context, exceptionInfo->ContextRecord, sizeof(CONTEXT));
    
    // Initialize stack frame
    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));
    
#if defined(_M_IX86)
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_X64)
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_IA64)
    DWORD machineType = IMAGE_FILE_MACHINE_IA64;
    stackFrame.AddrPC.Offset = context.StIIP;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.IntSp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrBStore.Offset = context.RsBSP;
    stackFrame.AddrBStore.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.IntSp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
    DWORD machineType = IMAGE_FILE_MACHINE_UNKNOWN;
#endif
    
    std::cerr << "Stack trace:\n";
    
    // Walk the stack
    for (int frameNum = 0; frameNum < 100; frameNum++) {
        BOOL result = StackWalk64(
            machineType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );
        
        if (!result || stackFrame.AddrPC.Offset == 0) {
            break;
        }
        
        // Get symbol information
        DWORD64 displacement = 0;
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol)) {
            // Try to get source line information
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            
            if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &line)) {
                std::cerr << "  [" << frameNum << "] " << symbol->Name << " + 0x"
                         << std::hex << displacement << std::dec << "\n";
                std::cerr << "      at " << line.FileName << ":" << line.LineNumber << "\n";
            } else {
                std::cerr << "  [" << frameNum << "] " << symbol->Name << " + 0x"
                         << std::hex << displacement << std::dec << "\n";
            }
        } else {
            std::cerr << "  [" << frameNum << "] 0x" << std::hex << stackFrame.AddrPC.Offset << std::dec << "\n";
        }
    }
    
    // Clean up
    SymCleanup(process);
}

void CrashHandler::printWindowsStacktrace() {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    // Initialize symbol handler
    SymInitialize(process, NULL, TRUE);
    
    // Get current context
    CONTEXT context;
    RtlCaptureContext(&context);
    
    // Initialize stack frame
    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));
    
#if defined(_M_IX86)
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_X64)
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_IA64)
    DWORD machineType = IMAGE_FILE_MACHINE_IA64;
    stackFrame.AddrPC.Offset = context.StIIP;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.IntSp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrBStore.Offset = context.RsBSP;
    stackFrame.AddrBStore.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.IntSp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
    DWORD machineType = IMAGE_FILE_MACHINE_UNKNOWN;
#endif
    
    std::cerr << "Stack trace:\n";
    
    // Walk the stack
    for (int frameNum = 0; frameNum < 100; frameNum++) {
        BOOL result = StackWalk64(
            machineType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );
        
        if (!result || stackFrame.AddrPC.Offset == 0) {
            break;
        }
        
        // Get symbol information
        DWORD64 displacement = 0;
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol)) {
            // Try to get source line information
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            
            if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &line)) {
                std::cerr << "  [" << frameNum << "] " << symbol->Name << " + 0x"
                         << std::hex << displacement << std::dec << "\n";
                std::cerr << "      at " << line.FileName << ":" << line.LineNumber << "\n";
            } else {
                std::cerr << "  [" << frameNum << "] " << symbol->Name << " + 0x"
                         << std::hex << displacement << std::dec << "\n";
            }
        } else {
            std::cerr << "  [" << frameNum << "] 0x" << std::hex << stackFrame.AddrPC.Offset << std::dec << "\n";
        }
    }
    
    // Clean up
    SymCleanup(process);
}

#endif

void CrashHandler::RegisterCrashHandler(std::function<void()> handler) {
    onCrashHandlers.push_back(handler);
}

void CrashHandler::CallCrashHandlers() {
    for (auto& handler : onCrashHandlers) {
        handler();
    }
}

} // namespace nyaa