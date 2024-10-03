
#ifndef LOGGER_H
#define LOGGER_H

/* Include standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/* Handle UNUSED_FUNC macro */
#if defined(__GNUC__) || defined(__clang__)
    #define UNUSED_FUNC __attribute__((unused))
#elif defined(_MSC_VER)
    #define UNUSED_FUNC __pragma(warning(suppress: 4505))
#else
    #define UNUSED_FUNC
#endif

/* Platform-specific includes and definitions */
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #define F_OK 0  /* For Windows compatibility */
    #define _SNPRINTF _snprintf
    #define _VSPRINTF _vsnprintf
    #define _STRDUP _strdup
    #define _LOCALTIME(t, tm) localtime_s(&(tm), &(t))
#else
    #include <unistd.h>
    #define _STRDUP strdup
    #define _LOCALTIME(t, tm) (tm = *localtime(&(t))) /* Fallback to non-thread-safe localtime */
#endif

/* Determine if snprintf and vsnprintf are available */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define HAVE_SNPRINTF 1
    #define HAVE_VSNPRINTF 1
#else
    #define HAVE_SNPRINTF 0
    #define HAVE_VSNPRINTF 0
#endif

/* Define _SNPRINTF and _VSPRINTF */
#if HAVE_SNPRINTF
    #define _SNPRINTF snprintf
    #define _VSPRINTF vsnprintf
#else
    /* Use macros to ignore size parameter */
    #define _SNPRINTF(buf, size, fmt, ...) sprintf(buf, fmt, __VA_ARGS__)
    #define _VSPRINTF(buf, size, fmt, args) vsprintf(buf, fmt, args)
#endif

/* Log levels */
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_FATAL 4

/* Default log level */
#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

/* Log file path and mode defaults */
#ifndef LOG_FILE_PATH
    #define LOG_FILE_PATH "default_log.log"
#endif

#ifndef LOG_FILE_MODE
    #define LOG_FILE_MODE "a"  /* "w" for write, "a" for append */
#endif

#define MAX_LOG_MESSAGE_LENGTH 2048  /* Increased buffer size */
#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

/* Logging modes */
typedef enum {
    LOG_MODE_FILE,
    LOG_MODE_STDERR,
    LOG_MODE_STDOUT
} LogMode;

/* Current settings for logging */
static int _log_level = LOG_LEVEL;
static FILE* _log_output_stream = NULL;
static char _log_file_path[256] = LOG_FILE_PATH;
static char _log_file_mode[5] = LOG_FILE_MODE;
static LogMode _log_mode = LOG_MODE_STDERR;

/* Function prototypes for log configuration */
static void _set_log_level(int level) UNUSED_FUNC;
static void _set_log_file_path(const char* filepath) UNUSED_FUNC;
static void _set_log_file_mode(const char* mode) UNUSED_FUNC;
static void _set_log_mode(LogMode mode) UNUSED_FUNC;
static void _open_log_file();
static void _close_log_file();
static void _log_message(int level, const char* file, int line, const char* format, ...) UNUSED_FUNC;
static const char* _log_level_to_string(int level);
static void _current_time_string(char* buffer, size_t size);
static void _clear_log_file() UNUSED_FUNC;  /* Function to clear the log file */
static void _erase_log_file() UNUSED_FUNC;  /* Function to erase the log file */

/* Set log level */
static void _set_log_level(int level) {
    _log_level = level;
}

/* Set log file path and reopen log file if necessary */
static void _set_log_file_path(const char* filepath) {
    strncpy(_log_file_path, filepath, sizeof(_log_file_path) - 1);
    _log_file_path[sizeof(_log_file_path) - 1] = '\0';
    if (_log_mode == LOG_MODE_FILE) {
        _open_log_file();
    }
}

/* Set log file mode and reopen log file if necessary */
static void _set_log_file_mode(const char* mode) {
    strncpy(_log_file_mode, mode, sizeof(_log_file_mode) - 1);
    _log_file_mode[sizeof(_log_file_mode) - 1] = '\0';
    if (_log_mode == LOG_MODE_FILE) {
        _open_log_file();
    }
}

/* Set log output mode (file, stderr, stdout) */
static void _set_log_mode(LogMode mode) {
    _log_mode = mode;
    if (_log_mode == LOG_MODE_FILE) {
        _open_log_file();
    } else {
        _close_log_file();
    }
}

/* Open the log file */
static void _open_log_file() {
    if (_log_output_stream) {
        _close_log_file();
    }

    _log_output_stream = fopen(_log_file_path, _log_file_mode);
    if (!_log_output_stream) {
        fprintf(stderr, "Failed to open log file %s: %s\n", _log_file_path, strerror(errno));
        _log_output_stream = stderr;  /* Fallback to stderr */
        _log_mode = LOG_MODE_STDERR;
    }
}

/* Close the log file if open */
static void _close_log_file() {
    if (_log_output_stream && _log_output_stream != stderr && _log_output_stream != stdout) {
        fclose(_log_output_stream);
        _log_output_stream = NULL;
    }
}

/* Convert log level to string */
static const char* _log_level_to_string(int level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_FATAL: return "FATAL";
        default:              return "UNKNOWN";
    }
}

/* Get current time string for log messages */
static void _current_time_string(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm tinfo;
    _LOCALTIME(now, tinfo);
    strftime(buffer, size, LOG_TIME_FORMAT, &tinfo);
}

/* Clear the log file */
static void _clear_log_file() {
    if (_log_mode == LOG_MODE_FILE && _log_output_stream) {
        FILE* new_stream = freopen(_log_file_path, "w", _log_output_stream);
        if (new_stream == NULL) {
            fprintf(stderr, "Failed to clear log file %s: %s\n", _log_file_path, strerror(errno));
            _log_output_stream = stderr;
            _log_mode = LOG_MODE_STDERR;
        } else {
            _log_output_stream = new_stream;
        }
    } else {
        fprintf(stderr, "Log file clearing failed. Current mode is not file mode.\n");
    }
}

/* Erase the log file */
static void _erase_log_file() {
    if (_log_mode == LOG_MODE_FILE && _log_output_stream) {
        _close_log_file();
        if (remove(_log_file_path) == 0) {
            _open_log_file();
        } else {
            fprintf(stderr, "Failed to erase log file %s: %s\n", _log_file_path, strerror(errno));
            _log_output_stream = stderr;
            _log_mode = LOG_MODE_STDERR;
        }
    } else {
        fprintf(stderr, "Log file erasing failed. Current mode is not file mode.\n");
    }
}

/* Log a message based on the current settings */
static void _log_message(int level, const char* file, int line, const char* format, ...) {
    if (level >= _log_level) {
        char log_message[MAX_LOG_MESSAGE_LENGTH];
        char time_buffer[64];
        va_list args;

        _current_time_string(time_buffer, sizeof(time_buffer));

        /* Create the initial part of the log message */
        _SNPRINTF(log_message, sizeof(log_message), "[%s] %s:%d [%s] ", time_buffer, file, line, _log_level_to_string(level));

        /* Append the formatted user message */
        va_start(args, format);
        _VSPRINTF(log_message + strlen(log_message), sizeof(log_message) - strlen(log_message), format, args);
        va_end(args);

        FILE* output_stream = (_log_mode == LOG_MODE_FILE) ? _log_output_stream :
                              (_log_mode == LOG_MODE_STDERR) ? stderr : stdout;

        fprintf(output_stream, "%s\n", log_message);
        fflush(output_stream);
    }
}

/* Logging macros for convenience */
#define LOG_DEBUG(...) _log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  _log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  _log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) _log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) _log_message(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* Setters for log configuration via macros */
#define SET_LOG_LEVEL(level) _set_log_level(level)
#define SET_LOG_FILE_PATH(filepath) _set_log_file_path(filepath)
#define SET_LOG_FILE_MODE(mode) _set_log_file_mode(mode)
#define SET_LOG_MODE(mode) _set_log_mode(mode)
#define CLEAR_LOG_FILE() _clear_log_file()
#define ERASE_LOG_FILE() _erase_log_file()

#endif /* LOGGER_H */
