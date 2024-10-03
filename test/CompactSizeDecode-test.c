#include "logger.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

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


// Define a struct for test cases
typedef struct {
    const char* testName;
    uint8_t *data;
    size_t dataLen;
    uint64_t expectedValue;
    size_t expectedBytes;
    int shouldFail;  // 0 for success, 1 for failure
} TestCase;

// Helper function to run a test case
void runTestCase(TestCase* test) {
    uint64_t actualValue = 0;
    size_t bytesRead = CompactSizeDecode(test->data, test->dataLen, &actualValue);

    printf("Running test: %s\n", test->testName);
    printf("Data: ");
    for (size_t i = 0; i < test->dataLen; i++) {
        printf("%02x ", test->data[i]);
    }
    printf("\nExpected value: %lu, Actual value: %lu\n", test->expectedValue, actualValue);
    printf("Expected bytes: %zu, Actual bytes: %zu\n", test->expectedBytes, bytesRead);

    if (!test->shouldFail) {
        assert(actualValue == test->expectedValue && "Value does not match expected");
        assert(bytesRead == test->expectedBytes && "Bytes read does not match expected");
    } else {
        assert(actualValue == 0 && "Value should be 0 on failure");
        assert(bytesRead == test->expectedBytes && "Bytes read should match dataSize on failure");
    }

    printf("Test %s passed!\n\n", test->testName);
}

int main() {
    printf("Starting extensive CompactSizeDecode test suite...\n\n");

    // Define all test cases in an array directly
	//
	// NOTE: Params are:
	// {"Test Name", (uint8_t[]){ ... }, dataLength, expectedValue, expectedBytesRead, shouldFail}
    TestCase testCases[] = {
        {"Single byte value (<= 0xFC)", (uint8_t[]){0xFC}, 1, 0xFC, 1, 0},
        {"Two-byte value (0xFD + 2 bytes)", (uint8_t[]){0xFD, 0x12, 0x34}, 3, 0x3412, 3, 0},
        {"Insufficient data for 2-byte value", (uint8_t[]){0xFD, 0x12}, 2, 0, 2, 1},
        {"Four-byte value (0xFE + 4 bytes)", (uint8_t[]){0xFE, 0x78, 0x56, 0x34, 0x12}, 5, 0x12345678, 5, 0},
        {"Insufficient data for 4-byte value", (uint8_t[]){0xFE, 0x12, 0x34}, 3, 0, 3, 1},
        {"Eight-byte value (0xFF + 8 bytes)", (uint8_t[]){0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}, 9, 0x0807060504030201, 9, 0},
        {"Insufficient data for 8-byte value", (uint8_t[]){0xFF, 0x01, 0x02, 0x03, 0x04}, 5, 0, 5, 1},
        {"Single byte value 0x00", (uint8_t[]){0x00}, 1, 0x00, 1, 0},
        {"Random garbage data", (uint8_t[]){0xAB, 0xCD, 0xEF}, 3, 0xAB, 1, 0},
        {"Max 2-byte value", (uint8_t[]){0xFD, 0xFF, 0xFF}, 3, 0xFFFF, 3, 0},
        {"Max 4-byte value", (uint8_t[]){0xFE, 0xFF, 0xFF, 0xFF, 0xFF}, 5, 0xFFFFFFFF, 5, 0},
        {"Max 8-byte value", (uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 9, 0xFFFFFFFFFFFFFFFF, 9, 0},
        {"Large dataset (random)", (uint8_t[]){0xAB, 0xAB, 0xCD, 0xEF}, 4, 171, 1, 0},  // Tests large random data but only expects 1 byte to be read
    };

    // Calculate the number of test cases
    int numTests = sizeof(testCases) / sizeof(TestCase);

    // Run all tests
    for (int i = 0; i < numTests; i++) {
        runTestCase(&testCases[i]);
    }

    printf("ALL TESTS COMPLETED SUCCESSFULLY!\n");

    return 0;
}
