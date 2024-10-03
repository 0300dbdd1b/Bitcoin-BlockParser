#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
	#define PATH_SEPARATOR '\\'
	#define __PATH_SEPARATOR_STR "\\"
	#define DEFAULT_BITCOIN_DIR "%LOCALAPPDATA%\\Bitcoin\\"
	#define MAX_PATH_LENGTH 500
#elif defined(__linux__)
    #define PLATFORM_LINUX
	#define __PATH_SEPARATOR_STR "/"
	#define PATH_SEPARATOR '/'
	#define DEFAULT_BITCOIN_DIR "$HOME/.bitcoin/"
	#define MAX_PATH_LENGTH 500
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_DARWIN
	#define PATH_SEPARATOR '/'
	#define __PATH_SEPARATOR_STR "/"
	#define DEFAULT_BITCOIN_DIR "$HOME/Library/Application Support/Bitcoin/"
	#define MAX_PATH_LENGTH 500
#else
    #error "Unsupported platform"
#endif

#if defined(__GNUC__)
    #define COMPILER_GCC
#elif defined(__clang__)
    #define COMPILER_CLANG
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#else
    #error "Unsupported compiler"
#endif

#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH_X86
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH_ARM
#elif defined(__aarch64__)
    #define ARCH_ARM64
#else
    #error "Unsupported architecture"
#endif

#define DEFAULT_BITCOIN_CONF				DEFAULT_BITCOIN_DIR "bitcoin.conf"
#define DEFAULT_BITCOIN_BLOCKS_DIR			DEFAULT_BITCOIN_DIR "blocks" __PATH_SEPARATOR_STR
#define DEFAULT_BITCOIN_BLOCKS_INDEX_DIR	DEFAULT_BITCOIN_BLOCKS_DIR "index" __PATH_SEPARATOR_STR
#define DEFAULT_BITCOIN_CHAINSTATE_DIR		DEFAULT_BITCOIN_DIR "chainstate" __PATH_SEPARATOR_STR
#define DEFAULT_BITCOIN_INDEXES_DIR			DEFAULT_BITCOIN_DIR "indexes" __PATH_SEPARATOR_STR
#define DEFAULT_BITCOIN_TXINDEX_DIR			DEFAULT_BITCOIN_INDEXES_DIR "txindex" __PATH_SEPARATOR_STR
#define DEFAULT_BITCOIN_COINSTATS_DIR		DEFAULT_BITCOIN_INDEXES_DIR "coinstats" __PATH_SEPARATOR_STR

#endif // PLATFORM_H
