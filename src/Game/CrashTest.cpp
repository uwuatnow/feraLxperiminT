#include "CrashTest.h"
#include "CrashHandler.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

namespace nyaa {

void CrashTest::testUnhandledException() {
    std::cout << "Testing unhandled exception...\n";
    CrashHandler::printCurrentStacktrace("Before throwing exception");
    
    throw std::runtime_error("This is a test exception to demonstrate uncaught exception handling");
}

void CrashTest::testSegmentationFault() {
    std::cout << "Testing segmentation fault...\n";
    CrashHandler::printCurrentStacktrace("Before segfault");
    
    // Cause a segmentation fault
    int* ptr = nullptr;
    *ptr = 42;  // This will crash
}

void CrashTest::testFloatingPointException() {
    std::cout << "Testing floating point exception...\n";
    CrashHandler::printCurrentStacktrace("Before division by zero");
    
    // Cause division by zero (may not always trigger SIGFPE on all systems)
    volatile int zero = 0;
    volatile int result = 42 / zero;
    (void)result;  // Suppress unused variable warning
}

void CrashTest::testAbort() {
    std::cout << "Testing abort signal...\n";
    CrashHandler::printCurrentStacktrace("Before abort");
    
    std::abort();  // This will trigger SIGABRT
}

void CrashTest::testNestedCalls() {
    std::cout << "Testing nested function calls for stacktrace depth...\n";
    CrashHandler::printCurrentStacktrace("Before nested calls");
    
    level1();
}

void CrashTest::level3() {
    CrashHandler::printCurrentStacktrace("In level3() - about to throw");
    throw std::logic_error("Test exception from nested call");
}

void CrashTest::level2() {
    level3();
}

void CrashTest::level1() {
    level2();
}

} // namespace nyaa