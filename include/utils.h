#ifndef UTILS_H
 #define UTILS_H
 #include "stdint.h"
 #include "structs.h"
#include <stdint.h>


// NOTE:------------------DIRUTILS-------------------------------------------
int IsDirectory(const char *path);
void SanitizeDirString(char *dir);

int CopyDirectory(const char *src_dir, const char *dest_dir);
int DeleteDirectory(const char *dir_path);

long GetFileSize(const char *filename);			// Get the file size in bytes
//FilePathList ListFiles(const char *directory, const char *pattern, uint16_t allocationSize);
FileList ListFiles(const char *directory, const char *pattern, uint16_t allocationSize);

// NOTE:------------------UTILS---------------------------------------------

// NOTE: CompactSize are used by Bitcoin(Core)
size_t CompactSizeEncode(uint64_t value, uint8_t* output);
// size_t CompactSizeDecode(const uint8_t* data, uint64_t *output, size_t dataSize);
size_t CompactSizeDecode(const uint8_t *data, size_t dataSize, uint64_t *value);

//NOTE: Custom Implementation of VarInt used by Bitcoin(Core) in leveldb
size_t DecodeVarint128(const void *data, uint64_t *value);
size_t EncodeVarint128(uint64_t value, uint8_t* data);

void ReverseString(unsigned char *str, size_t size);
// void PrintByteString(const uint8_t *array, size_t size);
void PrintByteStringUint32(const uint32_t *array, size_t size);
void ConvertUint32ToUint8Array(uint32_t value, uint8_t *outputArray);

void ConvertTimestampToString(uint32_t timestamp, char *buffer, size_t bufferSize);
void ConvertUint32ToUint8Array(uint32_t value, uint8_t *bytes);
void PrintByteString(const uint8_t *bytes, size_t size, FILE *output);

// NOTE:------------------BITCOINUTILS----------------------------------------

BlockHeader DecodeBlockHeader(const uint8_t *rawBlockHeader);

void PrintBlockHeader(const BlockHeader *header, FILE *output);
void PrintStackItem(const StackItem *item, FILE *output);
void PrintWitness(const Witness *witness, FILE *output);
void PrintOutput(const Output *output, FILE *file);
void PrintInput(const Input *input, FILE *file);
void PrintTransaction(const Transaction *tx, FILE *output);
void PrintBlock(const Block *block, FILE *output);

void DoubleSHA256(const uint8_t *data, size_t datasize, uint8_t *hash);

#endif // !UTILS_H
