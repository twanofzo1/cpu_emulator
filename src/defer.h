#pragma once


#if defined(DEFER)

    #if defined(_MSC_VER) // Check if compiling with MSVC

    #define DEFER_CONCAT_IMPL(a, b) a##b
    #define DEFER_CONCAT(a, b) DEFER_CONCAT_IMPL(a, b)

    #define defer(stmt) __pragma(warning(push)) __pragma(warning(disable : 4100)) /* No-op for MSVC */ __pragma(warning(pop))

    #else // For GCC/Clang

    #define DEFER_CONCAT_IMPL(a, b) a##b
    #define DEFER_CONCAT(a, b) DEFER_CONCAT_IMPL(a, b)

    #define defer(stmt) \
        void DEFER_CONCAT(_defer_func_, __LINE__)(void *_) { (void)_; stmt; } \
        __attribute__((cleanup(DEFER_CONCAT(_defer_func_, __LINE__)))) char DEFER_CONCAT(_defer_var_, __LINE__)

    #endif

#else
#warning "DEFER is not defined. Please define DEFER to enable defer functionality."
#define defer(stmt) 
#endif