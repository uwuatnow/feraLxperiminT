#pragma once

#include <string>
#include <exception>
#include <vector>
#include <functional>

// Forward declarations for Windows API types
#ifdef _WIN32
struct _EXCEPTION_POINTERS;
#endif

namespace nyaa {

class CrashHandler {
public:
    static void install();
    static void printStacktrace();
    static void printExceptionStacktrace(const std::exception& e);
    static void printCurrentStacktrace(const std::string& message = "");
    
private:
    static void setupSignalHandlers();
    static void setupUnhandledExceptionHandler();
    
    static void signalHandler(int signal);
    static void unhandledExceptionTerminateHandler();
    
#ifdef _WIN32
    static void setupWindowsUnhandledExceptionFilter();
    static long __stdcall windowsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo);
    static void printWindowsStacktrace(struct _EXCEPTION_POINTERS* exceptionInfo);
    static void printWindowsStacktrace();
#endif

    static void CallCrashHandlers();

    static std::vector<std::function<void()>> onCrashHandlers;

    static void RegisterCrashHandler(std::function<void()> handler);
};

} // namespace nyaa