#include "blkdat.h"
#include "logger.h"
#include "structs.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "sha256.h"

extern IndexRecords gIndexRecords;
extern FileList gBlkFiles;
static OpenFileCache openFiles[MAX_OPEN_FILES];

void InitBlockHeader(BlockHeader *header);
void InitInput(Input *input);
void InitOutput(Output *output);
void InitStackItem(StackItem *item);
void InitWitness(Witness *witness);
void InitTransaction(Transaction *tx);
void InitBlock(Block *block);

Transaction *ReadTxn(const uint8_t *blockBuffer, uint16_t txCount)
{
	// NOTE: To get the TXID we need to keep track of the raw transaction data without the witness fields.
	// WARN:Even though it works for 90% of transactions, MUST double check for edge cases.
	Transaction *transactions = (Transaction *)malloc(sizeof(Transaction) * txCount);
	SHA256_CTX ctx;
	size_t bufferOffset = 0;
	uint8_t isSegwit = 0;
	size_t startPosOffset;
	for (uint16_t i = 0; i < txCount; i++)
	{
		Transaction transaction;

		startPosOffset = bufferOffset;
		memcpy(&transaction, blockBuffer + bufferOffset, sizeof(transaction.version) + sizeof(transaction.marker) + sizeof(transaction.flag)); // 6 bytes
		bufferOffset += sizeof(transaction.version);
		if (transaction.marker == 0x00 && transaction.flag >= 0x01)
		{
			bufferOffset += sizeof(transaction.marker) + sizeof(transaction.flag);
			isSegwit = 1;
		}
		else
		{
			transaction.marker = 0;
			transaction.flag = 0;
			isSegwit = 0;
		}
		bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&transaction.inputCount);
		transaction.inputs = (Input *)malloc(sizeof(Input) * transaction.inputCount);
		
		for (uint16_t inputIndex = 0; inputIndex < transaction.inputCount; inputIndex++)
		{
			Input input;

			memcpy(&input, blockBuffer + bufferOffset, SHA256_HASH_SIZE + sizeof(input.vout));								//NOTE: Get the input TXID and VOUT
			bufferOffset += SHA256_HASH_SIZE + sizeof(input.vout);
			bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&input.scriptSigSize);	//NOTE: Get the ScriptSig Size

			input.scriptSig = (uint8_t *)malloc(sizeof(uint8_t) * input.scriptSigSize);

			memcpy(input.scriptSig, blockBuffer + bufferOffset, input.scriptSigSize);										//NOTE: Get the ScriptSig
			bufferOffset += input.scriptSigSize;
			memcpy(&input.sequence, blockBuffer + bufferOffset, sizeof(input.sequence) ); 									//NOTE: Get Sequence
			bufferOffset += sizeof(input.sequence);
			transaction.inputs[inputIndex] = input;
		}

		bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&transaction.outputCount);
		transaction.outputs = (Output *)malloc(sizeof(Output) * transaction.outputCount);
		for (uint16_t outputIndex = 0; outputIndex < transaction.outputCount; outputIndex++)
		{
			Output output;

			memcpy(&output.amount, blockBuffer + bufferOffset, sizeof(output.amount));
			bufferOffset += sizeof(output.amount);
			bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&output.scriptPubKeySize);
			output.scriptPubKey = (uint8_t *)malloc(sizeof(uint8_t) * output.scriptPubKeySize);
			memcpy(output.scriptPubKey, blockBuffer + bufferOffset, output.scriptPubKeySize);
			bufferOffset += output.scriptPubKeySize;
			transaction.outputs[outputIndex] = output;
		}
 
		if (isSegwit)
		{
			transaction.witnesses = malloc(sizeof(Witness) * transaction.inputCount);
			for (uint16_t witnessIndex = 0; witnessIndex < transaction.inputCount; witnessIndex++)
			{
				Witness witness;
				bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&witness.stackItemsCount);
				witness.stackItems = (StackItem *)malloc(sizeof(StackItem) * witness.stackItemsCount);
				for (uint16_t stackItemIndex = 0; stackItemIndex < witness.stackItemsCount; stackItemIndex++)
				{
					StackItem stackItem;

					bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&stackItem.size);
					stackItem.item = (uint8_t *)malloc(sizeof(uint8_t) * stackItem.size);
					memcpy(stackItem.item, blockBuffer + bufferOffset, stackItem.size);
					bufferOffset += stackItem.size;
					witness.stackItems[stackItemIndex] = stackItem;
				}
				transaction.witnesses[witnessIndex] = witness;
			}
		}
		memcpy(&transaction.locktime, blockBuffer + bufferOffset, sizeof(transaction.locktime));		//NOTE: Get Locktime
		bufferOffset += sizeof(transaction.locktime);
		transaction.size = (uint16_t)bufferOffset - startPosOffset;
		uint8_t *wtxBuffer = (uint8_t *)malloc(sizeof(uint8_t) * transaction.size);
		memcpy(wtxBuffer, blockBuffer + startPosOffset, transaction.size);
		DoubleSHA256(wtxBuffer, transaction.size, transaction.wtxid);
		
		memcpy(transaction.txid, transaction.wtxid, sizeof(transaction.wtxid));
		// PrintByteString(transaction.wtxid, 32);
		// printf("\n");
		free(wtxBuffer);
		transactions[i] = transaction;
	}
	return transactions;
}



void CloseLRUFile() {
    size_t lruIndex = 0;
    uint32_t oldestTime = openFiles[0].lastAccessTime;

    // Iterate over open files to find the least recently used (LRU)
    for (size_t i = 1; i < MAX_OPEN_FILES; i++) {
        if (openFiles[i].fileInfo != NULL && openFiles[i].lastAccessTime < oldestTime) {
            lruIndex = i;
            oldestTime = openFiles[i].lastAccessTime;
        }
    }

    // Close the least recently used file
    fclose(openFiles[lruIndex].fileInfo->file);
    openFiles[lruIndex].fileInfo->isOpen = 0;
    openFiles[lruIndex].fileInfo = NULL;  // Remove the entry from the cache
}

FILE* OpenFile(FileInfo *fileInfo)
{
    // Check if the file is already open in FileInfo itself
    if (fileInfo->isOpen)
	{
        // The file is already open, update access time in cache
        for (size_t i = 0; i < MAX_OPEN_FILES; i++)
		{
            if (openFiles[i].fileInfo == fileInfo)
			{
                openFiles[i].lastAccessTime = (uint32_t)time(NULL); //WARN: Not portable
                return fileInfo->file;  // Return the open file
            }
        }
    }

    // Check if we need to close an LRU file
    int openSlots = 0;
    for (size_t i = 0; i < MAX_OPEN_FILES; i++)
	{
        if (openFiles[i].fileInfo == NULL)
            openSlots++;  // Count available slots
    }

    // If no slots are available, close the least recently used file
    if (openSlots == 0)
        CloseLRUFile();

    // Actually open the file
    fileInfo->file = fopen(fileInfo->filepath, "r");
    if (fileInfo->file == NULL)
	{
        perror("Failed to open file");
        return NULL;
    }
    fileInfo->isOpen = 1;  // Mark the file as open

    // Add the file to the cache in the first available slot
    for (size_t i = 0; i < MAX_OPEN_FILES; i++)
	{
        if (openFiles[i].fileInfo == NULL)
		{
            openFiles[i].fileInfo = fileInfo;
            openFiles[i].lastAccessTime = (uint32_t)time(NULL); //WARN: Not portable
            break;
        }
    }

    return fileInfo->file;
}

Block ReadBlkDatFile(FileInfo *fileInfo, size_t offset)
{
	Block block;
	uint8_t *blockBuffer;
	uint8_t blockHeaderBuffer[BLOCKHEADER_SIZE];
	int 	freadReturn;

	uint16_t bufferOffset = BLOCKHEADER_SIZE;
	fileInfo->file = OpenFile(fileInfo);
    if (fseek(fileInfo->file, offset - 8, SEEK_SET) != 0)
		LOG_FATAL("Failed to seek to the offset");
	freadReturn = fread(&block, sizeof(block.magic) + sizeof(block.blockSize), 1, fileInfo->file);	//NOTE: Read magic bytes and blockSize
	if (offset - 8 + block.blockSize > MAX_BLOCKFILE_SIZE)
		LOG_FATAL("Trying to read bytes after the end of the file");
	blockBuffer = malloc(sizeof(uint8_t) * block.blockSize);
	if (fread(blockBuffer, sizeof(uint8_t), block.blockSize, fileInfo->file) != block.blockSize)	//NOTE: Read blockSize bytes into a buffer to gather the block data
		LOG_FATAL("Cannot Read.");
	if (ferror(fileInfo->file))
		LOG_FATAL("Error while reading the file");
	memcpy(blockHeaderBuffer, blockBuffer, BLOCKHEADER_SIZE);										//NOTE: Copy the header bytes to a buffer
	block.header = DecodeBlockHeader(blockHeaderBuffer);											//NOTE: Decode the blockheader buffer
	bufferOffset += CompactSizeDecode(blockBuffer + bufferOffset, MAX_COMPACT_SIZE_BYTES, (uint64_t *)&block.txCount);	//NOTE: Get the number of transactions from the blockBuffer
	block.transactions = ReadTxn(blockBuffer + bufferOffset, block.txCount);						//NOTE: Get all the block transactions

	DoubleSHA256(blockHeaderBuffer, BLOCKHEADER_SIZE, block.hash);
	free(blockBuffer);
	return block;
}	
