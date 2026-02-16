#pragma once

#include <stdexcept>

namespace nyaa {

class CrashTest {
public:
    // Test function that throws an exception
    static void testUnhandledException();
    
    // Test function that causes a segmentation fault
    static void testSegmentationFault();
    
    // Test function that causes a division by zero
    static void testFloatingPointException();
    
    // Test function that calls abort
    static void testAbort();
    
    // Test nested function calls to show stacktrace depth
    static void testNestedCalls();
    
private:
    static void level3();
    static void level2();
    static void level1();
};

} // namespace nyaa