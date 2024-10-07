#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "sha256.h"
#include "time.h"

#include "utils.h"
#include "structs.h"

void SHA256(const uint8_t *data, size_t datasize, uint8_t *hash)
{
	SHA256_CTX ctx;

	sha256_init(&ctx);
	sha256_update(&ctx, data, datasize);
	sha256_final(&ctx, hash);
	ReverseString(hash, 32);
}

void DoubleSHA256(const uint8_t *data, size_t datasize, uint8_t *hash)
{
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, datasize);
	sha256_final(&ctx, hash);

	sha256_init(&ctx);
	sha256_update(&ctx, hash, 32);
	sha256_final(&ctx, hash);
	ReverseString(hash, 32);
}

void ConvertTimestampToString(unsigned int timestamp, char *buffer, size_t bufferSize)
{
    time_t rawTime = (time_t)timestamp; // Convert the 4-byte timestamp to time_t
    struct tm *timeInfo;

    // Convert raw time to struct tm
    timeInfo = localtime(&rawTime);

    // Format time into a string (e.g., "YYYY-MM-DD HH:MM:SS")
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", timeInfo);
}

uint32_t ChangeEndiannessUint32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |  // Move byte 3 to byte 0
           ((value >> 8)  & 0x0000FF00) |  // Move byte 2 to byte 1
           ((value << 8)  & 0x00FF0000) |  // Move byte 1 to byte 2
           ((value << 24) & 0xFF000000);   // Move byte 0 to byte 3
}


// WARN: Must check for endianness before storing the value
BlockHeader DecodeBlockHeader(const uint8_t *rawBlockHeader)
{
	BlockHeader blockHeader;

	size_t offset = 0;

	// Extract Version (4 bytes, little-endian)
	memcpy(&blockHeader.version, rawBlockHeader + offset, 4);
	offset += 4;
	//blockHeader.version = ChangeEndiannessUint32(blockHeader.version);

	// Extract Previous Block Hash (32 bytes, natural byte order)
	memcpy(blockHeader.prevBlockHash, rawBlockHeader + offset, SHA256_HASH_SIZE);
	ReverseString(blockHeader.prevBlockHash, SHA256_HASH_SIZE);
	offset += 32;
	// Extract Merkle Root (32 bytes, natural byte order)
	memcpy(blockHeader.merkleRoot, rawBlockHeader + offset, SHA256_HASH_SIZE);
	ReverseString(blockHeader.merkleRoot, SHA256_HASH_SIZE);
	offset += 32;
	// Extract Time (4 bytes, little-endian)
	memcpy(&blockHeader.time, rawBlockHeader + offset, 4);
	offset += 4;
	// Extract Bits (4 bytes, little-endian)
	memcpy(&blockHeader.bits, rawBlockHeader + offset, 4);
	offset += 4;
	// Extract Nonce (4 bytes, little-endian)
	memcpy(&blockHeader.nonce, rawBlockHeader + offset, 4);

	return  blockHeader;
}




void PrintBlockHeader(const BlockHeader *header, FILE *output) {
    if (header == NULL) {
        fprintf(output, "\"BlockHeader\": null\n");
        return;
    }

    char buffer[50];
    ConvertTimestampToString(header->time, buffer, sizeof(buffer));

    uint8_t versionBytes[4], bitsBytes[4], nonceBytes[4];
    ConvertUint32ToUint8Array(header->version, versionBytes);
    ConvertUint32ToUint8Array(header->bits, bitsBytes);
    ConvertUint32ToUint8Array(header->nonce, nonceBytes);

    // Start BlockHeader JSON object
    fprintf(output, "\"BlockHeader\": {\n");
    fprintf(output, "  \"Version\": \"");
    PrintByteString(versionBytes, 4, output);
    fprintf(output, "\",\n");

    fprintf(output, "  \"PreviousBlockHash\": \"");
    PrintByteString(header->prevBlockHash, SHA256_HASH_SIZE, output);
    fprintf(output, "\",\n");

    fprintf(output, "  \"MerkleRoot\": \"");
    PrintByteString(header->merkleRoot, SHA256_HASH_SIZE, output);
    fprintf(output, "\",\n");

    fprintf(output, "  \"Time\": \"%s\",\n", buffer);

    fprintf(output, "  \"Bits\": \"");
    PrintByteString(bitsBytes, 4, output);
    fprintf(output, "\",\n");

    fprintf(output, "  \"Nonce\": \"");
    PrintByteString(nonceBytes, 4, output);
    fprintf(output, "\"\n}\n");
}




void PrintInput(const Input *input, FILE *output) {
    fprintf(output, "    {\n");
    fprintf(output, "      \"TXID\": \"");
    for (int i = 31; i >= 0; --i) {
        fprintf(output, "%02x", input->txid[i]);
    }
    fprintf(output, "\",\n");

    fprintf(output, "      \"Vout\": %u,\n", input->vout);
    fprintf(output, "      \"ScriptSigSize\": %u,\n", input->scriptSigSize);

    char *scriptSigHex = malloc(input->scriptSigSize * 2 + 1); // Ensure buffer is correct size
    if (scriptSigHex == NULL) {
        fprintf(stderr, "Memory allocation failed for scriptSigHex\n");
        return;
    }

    for (uint16_t i = 0, offset = 0; i < input->scriptSigSize; ++i) {
        snprintf(scriptSigHex + offset, 3, "%02x", input->scriptSig[i]);
        offset += 2;
    }
    scriptSigHex[input->scriptSigSize * 2] = '\0'; // Null-terminate the hex string

    fprintf(output, "      \"ScriptSig\": \"%s\",\n", scriptSigHex);
    free(scriptSigHex);

    fprintf(output, "      \"Sequence\": %u\n    }", input->sequence);
}

void PrintOutput(const Output *output, FILE *outputFile) {
    fprintf(outputFile, "    {\n");
    fprintf(outputFile, "      \"Amount\": %llu,\n", (unsigned long long)output->amount);
    fprintf(outputFile, "      \"ScriptPubKeySize\": %u,\n", output->scriptPubKeySize);

    char *scriptPubKeyHex = malloc(output->scriptPubKeySize * 2 + 1); // Ensure buffer is correct size
    if (scriptPubKeyHex == NULL) {
        fprintf(stderr, "Memory allocation failed for scriptPubKeyHex\n");
        return;
    }

    for (uint16_t i = 0, offset = 0; i < output->scriptPubKeySize; ++i) {
        snprintf(scriptPubKeyHex + offset, 3, "%02x", output->scriptPubKey[i]);
        offset += 2;
    }
    scriptPubKeyHex[output->scriptPubKeySize * 2] = '\0'; // Null-terminate the hex string

    fprintf(outputFile, "      \"ScriptPubKey\": \"%s\"\n    }", scriptPubKeyHex);
    free(scriptPubKeyHex);
}


void PrintTransaction(const Transaction *tx, FILE *output) {
    fprintf(output, "  {\n");
    fprintf(output, "    \"TransactionSize\": %d,\n", tx->size);
    fprintf(output, "    \"Version\": %u,\n", tx->version);

    // Print SegWit status
    fprintf(output, "    \"SegWit\": %s,\n", (tx->marker == 0x00 && tx->flag == 0x01) ? "true" : "false");

    fprintf(output, "    \"Inputs\": [\n");
    for (uint16_t i = 0; i < tx->inputCount; ++i) {
        PrintInput(&tx->inputs[i], output);
        if (i < tx->inputCount - 1)
            fprintf(output, ",\n");  // Add comma if not the last input
        else
            fprintf(output, "\n");
    }
    fprintf(output, "    ],\n");

    fprintf(output, "    \"Outputs\": [\n");
    for (uint16_t i = 0; i < tx->outputCount; ++i) {
        PrintOutput(&tx->outputs[i], output);
        if (i < tx->outputCount - 1)
            fprintf(output, ",\n");  // Add comma if not the last output
        else
            fprintf(output, "\n");
    }
    fprintf(output, "    ]\n  }");
}

void PrintBlock(const Block *block, FILE *output) {
    fprintf(output, "{\n");
    fprintf(output, "  \"Magic\": \"0x%08x\",\n", block->magic);
    fprintf(output, "  \"BlockSize\": %u,\n", block->blockSize);
    fprintf(output, "  \"TransactionCount\": %u,\n", block->txCount);
    
    // Print BlockHeader
    PrintBlockHeader(&block->header, output);

    // Add the missing comma after BlockHeader before Transactions
    fprintf(output, ",\n");

    // Print Transactions array
    fprintf(output, "  \"Transactions\": [\n");
    for (uint16_t i = 0; i < block->txCount; ++i) {
        PrintTransaction(&block->transactions[i], output);
        if (i < block->txCount - 1)
            fprintf(output, ",\n");  // Add comma if not the last transaction
        else
            fprintf(output, "\n");
    }
    fprintf(output, "  ]\n");
    fprintf(output, "}\n");
}
// Initializes BlockHeader with default values
void InitBlockHeader(BlockHeader *header) {
    memset(header, 0, sizeof(BlockHeader));  // Set everything to zero
    header->version = 0;  // You can assign meaningful default values
}

// Initializes Input and sets pointers to NULL
void InitInput(Input *input) {
    memset(input, 0, sizeof(Input));  // Set everything to zero
    input->scriptSig = NULL;  // Initialize pointer to NULL
}

// Initializes Output and sets pointers to NULL
void InitOutput(Output *output) {
    memset(output, 0, sizeof(Output));  // Set everything to zero
    output->scriptPubKey = NULL;  // Initialize pointer to NULL
}

// Initializes StackItem and sets pointers to NULL
void InitStackItem(StackItem *item) {
    item->size = 0;
    item->item = NULL;  // Initialize pointer to NULL
}

// Initializes Witness and sets pointers to NULL
void InitWitness(Witness *witness) {
    witness->stackItemsCount = 0;
    witness->stackItems = NULL;  // Initialize pointer to NULL
}

// Initializes Transaction and sets pointers to NULL
void InitTransaction(Transaction *tx) {
    memset(tx, 0, sizeof(Transaction));  // Set everything to zero
    tx->inputs = NULL;  // Initialize pointer to NULL
    tx->outputs = NULL;  // Initialize pointer to NULL
    tx->witnesses = NULL;  // Initialize pointer to NULL
}

// Initializes Block and sets pointers to NULL
void InitBlock(Block *block) {
    memset(block, 0, sizeof(Block));  // Set everything to zero
    InitBlockHeader(&block->header);  // Initialize the header
    block->transactions = NULL;  // Initialize pointer to NULL
}
