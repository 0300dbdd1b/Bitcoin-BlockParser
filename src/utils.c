#include <dirent.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "structs.h"

long GetFileSize(const char *filename)
{
	struct stat st;

	if (stat(filename, &st) == 0)
		return st.st_size;
	else
	{
		perror("stat");
		return -1;
	}
}


int WildcardMatch(const char *str, const char *pattern)
{
    const char *p = pattern;
    const char *s = str;
    const char *star = NULL;
    const char *ss = NULL;

	// WARN: Trust the process... (Don't)
    while (*s) {
        if (*p == '?' || *p == *s) {
            p++;
            s++;
        } else if (*p == '*') {
            star = p++;
            ss = s;
        } else if (star) {
            p = star + 1;
            s = ++ss;
        } else {
            return 0;
        }
    }

    while (*p == '*') {
        p++;
    }

    return *p == '\0';
}




// Encode an integer as a varint.
size_t VarintEncode(uint32_t value, uint8_t *output)
{
    size_t i = 0;
    while (value >= 0x80)
	{
        output[i++] = (value & 0x7F) | 0x80;  // Set the continuation bit
        value >>= 7;
    }
    output[i++] = value;  // Last byte, without the continuation bit
    return i;
}


// Decode a varint into an integer.
size_t VarintDecode(const uint8_t *input, uint32_t *output)
{
    uint32_t result = 0;
    size_t shift = 0;
    size_t i = 0;

    while (input[i] & 0x80)
	{
        result |= (input[i] & 0x7F) << shift;
        shift += 7;
        i++;
    }
    result |= input[i] << shift;
    i++;  // Increment to account for the last byte read
    if (output != NULL) {
        *output = result;
    }
    return i;
}



// WARN: CompactSize functions are untested yet, if ther'es a bug it probably comes from here.

// Function to encode a number into a compact size field
size_t CompactSizeEncode(uint64_t value, uint8_t* output) {
    if (value <= 252) {
        // Single-byte value
        output[0] = (uint8_t)value;
        return 1; // 1 byte written
    } else if (value <= 0xFFFF) {
        // FD prefix with 2 bytes
        output[0] = 0xFD;
        output[1] = (uint8_t)(value & 0xFF);
        output[2] = (uint8_t)((value >> 8) & 0xFF);
        return 3; // 3 bytes written
    } else if (value <= 0xFFFFFFFF) {
        // FE prefix with 4 bytes
        output[0] = 0xFE;
        output[1] = (uint8_t)(value & 0xFF);
        output[2] = (uint8_t)((value >> 8) & 0xFF);
        output[3] = (uint8_t)((value >> 16) & 0xFF);
        output[4] = (uint8_t)((value >> 24) & 0xFF);
        return 5; // 5 bytes written
    } else {
        // FF prefix with 8 bytes
        output[0] = 0xFF;
        output[1] = (uint8_t)(value & 0xFF);
        output[2] = (uint8_t)((value >> 8) & 0xFF);
        output[3] = (uint8_t)((value >> 16) & 0xFF);
        output[4] = (uint8_t)((value >> 24) & 0xFF);
        output[5] = (uint8_t)((value >> 32) & 0xFF);
        output[6] = (uint8_t)((value >> 40) & 0xFF);
        output[7] = (uint8_t)((value >> 48) & 0xFF);
        output[8] = (uint8_t)((value >> 56) & 0xFF);
        return 9; // 9 bytes written
    }
}


size_t CompactSizeDecode(const uint8_t *data, size_t dataSize, uint64_t *value) {
    if (data == NULL || value == NULL || dataSize == 0)
	{
        *value = 0;
        return 0;
    }

    uint8_t first = data[0];

    if (first <= 0xFC)
	{
        *value = first;
        return 1;
    }
	else if (first == 0xFD)
	{
        if (dataSize < 3)
		{
            // Not enough data, but function should not fail
            *value = 0;
            return dataSize;
        }
        // Value is in the next 2 bytes (little-endian)
        *value = ((uint64_t)data[1]) |
                 ((uint64_t)data[2] << 8);
        return 3;
    }
	else if (first == 0xFE)
	{
        // Check if there are enough bytes to read
        if (dataSize < 5)
		{
            *value = 0;
            return dataSize;
        }
        // Value is in the next 4 bytes (little-endian)
        *value = ((uint64_t)data[1]) |
                 ((uint64_t)data[2] << 8) |
                 ((uint64_t)data[3] << 16) |
                 ((uint64_t)data[4] << 24);
        return 5;
    }
	else if (first == 0xFF)
	{
        // Check if there are enough bytes to read
        if (dataSize < 9)
		{
            *value = 0;
            return dataSize;
        }
        // Value is in the next 8 bytes (little-endian)
        *value = ((uint64_t)data[1]) |
                 ((uint64_t)data[2] << 8) |
                 ((uint64_t)data[3] << 16) |
                 ((uint64_t)data[4] << 24) |
                 ((uint64_t)data[5] << 32) |
                 ((uint64_t)data[6] << 40) |
                 ((uint64_t)data[7] << 48) |
                 ((uint64_t)data[8] << 56);
        return 9;
    }
	else
	{
        // Should never reach here, but set value to 0
        *value = 0;
        return 1;
    }
}


void ReverseString(unsigned char *str, size_t size)
{
    // Swap characters from the two ends towards the middle
    for (size_t i = 0, j = size - 1; i < j; i++, j--)
	{
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}


void PrintByteString(const uint8_t *bytes, size_t size, FILE *output) {
    for (size_t i = 0; i < size; ++i) {
        fprintf(output, "%02X", bytes[i]);
    }
}


// Function to convert a uint32_t value to an array of 4 uint8_t values
void ConvertUint32ToUint8Array(uint32_t value, uint8_t *outputArray) {
    outputArray[0] = (value >> 24) & 0xFF;  // Most significant byte
    outputArray[1] = (value >> 16) & 0xFF;  // Second byte
    outputArray[2] = (value >> 8)  & 0xFF;  // Third byte
    outputArray[3] = (value)       & 0xFF;  // Least significant byte
}


// Function to encode a custom Bitcoin-style varint
// Returns the number of bytes used
size_t EncodeVarint128(uint64_t value, uint8_t* data) {
    uint8_t buffer[10];
    int i = 0;

    while (value > 0x7F) {
        buffer[i++] = (value & 0x7F) | 0x80;
        value = (value >> 7) - 1;
    }
    buffer[i++] = (uint8_t)(value & 0x7F);

    // Write the buffer in reverse order and count the number of bytes written
    for (int j = i - 1; j >= 0; j--) {
        *data++ = buffer[j];
    }

    return i;  // Returns the number of bytes used to encode the varint
}

size_t DecodeVarint128(const void *data, uint64_t *value) {
    const uint8_t *ret = (const uint8_t *)data; // Cast the void pointer to a single byte pointer
    const uint8_t *start = ret;                // Remember the start position to calculate the number of bytes read

    *value = 0;       // Set the return value to zero initially
    uint8_t byte = 0; // Each byte we are streaming in
    do {
        byte = *ret++; // Get the byte value and or the 7 lower bits into the result.
        *value = (*value << 7) | (uint64_t)(byte & 0x7F);
        // If this is not the last byte, we increment the value we are accumulating by one.
        // This is extremely non-standard and I could not find a reference to this technique
        // anywhere online other than the bitcoin source code itself.

        if (byte & 0x80) {
            (*value)++;
        }
    } while (byte & 0x80);

    return ret - start; // Return the number of bytes read
}
