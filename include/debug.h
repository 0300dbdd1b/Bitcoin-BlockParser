
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <time.h>


#if defined(__GNUC__) || defined(__clang__)
    #define UNUSED_FUNC __attribute__((unused))
#elif defined(_MSC_VER)
    #define UNUSED_FUNC __pragma(warning(suppress: 4505))
#else
    #define UNUSED_FUNC
#endif

#if defined(__has_include)
	#if __has_include("logger.h")
		#include "logger.h"
	#endif
#endif

// Macro to enable or disable debug mode
#ifndef DEBUG_MODE
    #define DEBUG_MODE 1  // 1 to enable debug features, 0 to disable
#endif

// Enumeration for different timing methods
typedef enum {
    TIMING_HIGH_RESOLUTION,
    TIMING_CLOCK,
    TIMING_RDTSC,
    TIMING_RDTSC_P
} TimingMethod;

// Function pointer type for timing functions
typedef double (*timing_function_t)();

// Function prototypes for timing methods
static double _get_current_time_ms();
static double _get_clock_time();
#ifdef __GNUC__  // Only include these if using GCC/Clang
static double _get_rdtsc_time();
static double _get_rdtscp_time();
#endif

// Function pointer to the chosen timing function
static timing_function_t _timing_function = _get_current_time_ms;

// Cross-platform high-resolution time function
#ifdef _WIN32
    #include <windows.h>
    
    static double _get_current_time_ms() {
        LARGE_INTEGER frequency;
        LARGE_INTEGER currentTime;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&currentTime);
        return (double)(currentTime.QuadPart * 1000.0 / frequency.QuadPart);
    }
#else
    #include <sys/time.h>
    
    static double _get_current_time_ms() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (double)(tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);
    }
#endif

// Using clock() for timing
static double _get_clock_time() {
    return (double)clock() / CLOCKS_PER_SEC * 1000.0;
}

// Using rdtsc for timing (only works on x86 processors, GCC/Clang compilers)
#ifdef __GNUC__  // Only define these if using GCC/Clang
    static double _get_rdtsc_time() {
        unsigned long long rax, rdx;
        __asm__ volatile ("rdtsc" : "=a" (rax), "=d" (rdx));
        return (double)((rdx << 32) + rax);
    }

    static double _get_rdtscp_time() {
        unsigned long long rax, rdx;
        __asm__ volatile ("rdtscp" : "=a" (rax), "=d" (rdx) : : "%rcx");
        return (double)((rdx << 32) + rax);
    }
#else
    #warning "RDTSC timing methods not supported on this compiler."
#endif

// Setter to change the timing method dynamically
static void set_timing_method(TimingMethod method) UNUSED_FUNC;
static void set_timing_method(TimingMethod method)
{
    switch (method)
	{
        case TIMING_HIGH_RESOLUTION:
            _timing_function = _get_current_time_ms;
            break;
        case TIMING_CLOCK:
            _timing_function = _get_clock_time;
            break;
#ifdef __GNUC__  // Only allow RDTSC if we're using GCC/Clang
        case TIMING_RDTSC:
            _timing_function = _get_rdtsc_time;
            break;
        case TIMING_RDTSC_P:
            _timing_function = _get_rdtscp_time;
            break;
#endif
        default:
            _timing_function = _get_current_time_ms;  // Default to high-resolution timing
            break;
    }
}

// Setter to change debug mode dynamically
static int _debug_mode = DEBUG_MODE;
static void set_debug_mode(int mode) UNUSED_FUNC;
static void set_debug_mode(int mode)
{
    _debug_mode = mode;
}

// Function to log or print timing result based on availability of logger.h
static void _debug_log_time(const char* message, double elapsed)
{
#ifdef LOGGER_H
    LOG_DEBUG("%s took %.2f ms", message, elapsed);
#else
    printf("%s took %.2f ms\n", message, elapsed);
#endif
}

// Timing Macros for Code Blocks

// Start timing a code block
#define TIME_BLOCK_START(var) \
    double var = _timing_function();

// End timing a code block and log the result
#define TIME_BLOCK_END(var, message) \
    do { \
        double _end_time = _timing_function(); \
        double _elapsed = _end_time - var; \
        _debug_log_time(message, _elapsed); \
    } while (0)

// Conditional debug macros
#if DEBUG_MODE
    #ifdef LOGGER_H
        #define DEBUG_PRINT(...) LOG_DEBUG(__VA_ARGS__)
    #else
        #define DEBUG_PRINT(...) \
            do { \
                fprintf(stderr, __VA_ARGS__); \
                fprintf(stderr, "\n"); \
            } while (0)
    #endif
#else
    #define DEBUG_PRINT(...)  // No-op if DEBUG_MODE is not enabled
#endif

// Benchmarking Macros

// Benchmark a function by running it multiple times and computing the average execution time
#define BENCHMARK(func, iterations) \
    do { \
        double _total_time = 0.0; \
        int _i; \
        for (_i = 0; _i < iterations; ++_i) { \
            double _start_time = _timing_function(); \
            func; \
            double _end_time = _timing_function(); \
            _total_time += (_end_time - _start_time); \
        } \
        double _average_time = _total_time / iterations; \
        _debug_log_time("Benchmark average time for " #func, _average_time); \
    } while (0)

#endif // DEBUG_H

