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

// Updated PrintBlockHeader function
void PrintBlockHeader(const BlockHeader *header) {
    if (header == NULL) {
        printf("BlockHeader is NULL.\n");
        return;
    }
    char buffer[50];
    ConvertTimestampToString((header->time), buffer, sizeof(buffer));
    // Convert version, bits, and nonce to uint8_t arrays for hex printing
    uint8_t versionBytes[4], bitsBytes[4], nonceBytes[4];
    ConvertUint32ToUint8Array((header->version), versionBytes);
    ConvertUint32ToUint8Array((header->bits), bitsBytes);
    ConvertUint32ToUint8Array((header->nonce), nonceBytes);

    // Print version in hex format
    printf("Version: ");
    PrintByteString(versionBytes, 4);
    printf("\n");

    // Print previous block hash in hex format
    printf("Previous Block Hash: ");
    PrintByteString(header->prevBlockHash, SHA256_HASH_SIZE);
    printf("\n");

    // Print Merkle root in hex format
    printf("Merkle Root: ");
    PrintByteString(header->merkleRoot, SHA256_HASH_SIZE);
    printf("\n");

    // Print time as a formatted string
    printf("Time: %s\n", buffer);

    // Print bits in hex format
    printf("Bits: ");
    PrintByteString(bitsBytes, 4);
    printf("\n");

    // Print nonce in hex format
    printf("Nonce: ");
    PrintByteString(nonceBytes, 4);
    printf("\n\n");
}

void PrintStackItem(const StackItem *item)
{
    // Print the size of the stack item
    printf("      Size: %u bytes\n", item->size);

    // Print the item data in hexadecimal
    printf("      Data: ");
    for (uint16_t i = 0; i < item->size; ++i)
        printf("%02x", item->item[i]);
    printf("\n");
}

void PrintWitness(const Witness *witness)
{
    printf("    Stack Items Count: %u\n", witness->stackItemsCount);

    for (uint16_t i = 0; i < witness->stackItemsCount; ++i)
    {
        printf("    Stack Item %u:\n", i + 1);
        PrintStackItem(&witness->stackItems[i]);
    }
}

// Function to print an output
void PrintOutput(const Output *output)
{
    // Print the amount in satoshis
    printf("  Amount: %llu satoshis\n", (unsigned long long)output->amount);

    // Print the scriptPubKey size
    printf("  ScriptPubKey Size: %u bytes\n", output->scriptPubKeySize);

    // Print the scriptPubKey in hexadecimal
    printf("  ScriptPubKey: ");
    for (uint16_t i = 0; i < output->scriptPubKeySize; ++i)
        printf("%02x", output->scriptPubKey[i]);
    printf("\n");
}

// Function to print an input
void PrintInput(const Input *input)
{
    // Print the transaction ID (txid) in reverse order (big-endian format)
    printf("  TXID: ");
    for (int i = 31; i >= 0; --i)
        printf("%02x", input->txid[i]);
    printf("\n");

    // Print the output index (vout)
    printf("  Vout: %u\n", input->vout);

    // Print the scriptSig size
    printf("  ScriptSig Size: %u bytes\n", input->scriptSigSize);

    // Print the scriptSig in hexadecimal
    printf("  ScriptSig: ");
    for (uint16_t i = 0; i < input->scriptSigSize; ++i)
        printf("%02x", input->scriptSig[i]);
    printf("\n");

    // Print the sequence number
    printf("  Sequence: %u\n", input->sequence);
}

void PrintTransaction(const Transaction *tx)
{
    // Print the transaction version
	printf("Transaction Size: %d\n", tx->size);
    printf("Version: %u\n", tx->version);

    // Check for SegWit marker and flag
    if (tx->marker == 0x00 && tx->flag == 0x01)
    {
        printf("SegWit Transaction Detected\n");
    }
    else
    {
        printf("Non-SegWit Transaction\n");
    }

    // Print the number of inputs
    printf("Input Count: %u\n", tx->inputCount);

    // Loop through each input and print its details
    for (uint16_t i = 0; i < tx->inputCount; ++i)
    {
        printf("Input %u:\n", i + 1);
        PrintInput(&tx->inputs[i]);
    }

    // Print the number of outputs
    printf("Output Count: %u\n", tx->outputCount);

    // Loop through each output and print its details
    for (uint16_t i = 0; i < tx->outputCount; ++i)
    {
        printf("Output %u:\n", i + 1);
        PrintOutput(&tx->outputs[i]);
    }

    // If it's a SegWit transaction, print the witness data
    if (tx->marker == 0x00 && tx->flag == 0x01)
    {
        printf("Witness Data:\n");
        for (uint16_t i = 0; i < tx->inputCount; ++i)
        {
            printf("Witness for Input %u:\n", i + 1);
            PrintWitness(&tx->witnesses[i]);
        }
    }

    // Print the locktime
    printf("Locktime: %u\n", tx->locktime);
}

void PrintBlock(const Block *block)
{
    // Print the magic number in hexadecimal format
    printf("Magic: 0x%08x\n", block->magic);

    // Print the block size in bytes
    printf("Block Size: %u bytes\n", block->blockSize);

    // Print the number of transactions in the block
    printf("Transaction Count: %u\n", block->txCount);

    // Print the block header
    printf("Block Header:\n");
    PrintBlockHeader(&block->header);

    // Print each transaction in the block
    printf("Transactions:\n");
    for (uint16_t i = 0; i < block->txCount; ++i)
    {
        printf("Transaction %u:\n", i + 1);
        PrintTransaction(&block->transactions[i]);
    }
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
