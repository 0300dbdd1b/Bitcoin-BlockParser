#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-truncation"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4996)  // Example warning code for unsafe functions in MSVC
#endif


#include "debug.h"
#include "logger.h"
#include "platform.h"
#include "structs.h"
#include "leveldb.h"
#include "utils.h"
#include "blkdat.h"

#include "stdio.h"
#include "stdlib.h"

#include <stdint.h>
#include <string.h>

extern FileList gBlkFiles;
extern IndexRecords gIndexRecords;

void PrintBlockIndexRecord(const BlockIndexRecord *record);

BlockIndexRecord GetBlockIndexRecord(const char *key, size_t keyLen, const char *value, size_t valLen)
{
	BlockIndexRecord blockIndexRecord;
	size_t offset = 0;

	//NOTE: Storing the blockHash in the BlockIndexRecord
	memcpy(blockIndexRecord.blockHash, key + 1, SHA256_HASH_SIZE);
 	ReverseString(blockIndexRecord.blockHash, SHA256_HASH_SIZE);
	
	offset += DecodeVarint128(value, &blockIndexRecord.version);					// Client version
	offset += DecodeVarint128(value + offset, &blockIndexRecord.height);			// Block Height
	offset += DecodeVarint128(value + offset, (uint64_t *)&blockIndexRecord.validationStatus);	// Validation Status
	offset += DecodeVarint128(value + offset, &blockIndexRecord.txCount);			// Number of transaction
	offset += DecodeVarint128(value + offset, &blockIndexRecord.blockFile);			// in which .ldb file block is stored
	offset += DecodeVarint128(value + offset, &blockIndexRecord.blockOffset);		// location of the block in the .ldb file
	
	if (offset < (valLen - BLOCKHEADER_SIZE))
	{
		offset += DecodeVarint128(value + offset, &blockIndexRecord.undoFile);
		offset += DecodeVarint128(value + offset, &blockIndexRecord.undoOffset);
	}

	if (valLen >= BLOCKHEADER_SIZE)
		blockIndexRecord.blockHeader = DecodeBlockHeader((const uint8_t *)value + (valLen - BLOCKHEADER_SIZE));
	return blockIndexRecord;
}

	

FileInformationRecord GetFileInformationRecord(const char *key, size_t keyLen, const char *value, size_t valLen)
{
	FileInformationRecord fileInformationRecord;
	size_t offset = 0;

	offset += DecodeVarint128(value + offset, &fileInformationRecord.blockCount);
	offset += DecodeVarint128(value + offset, &fileInformationRecord.dataFileSize);
	offset += DecodeVarint128(value + offset, &fileInformationRecord.revFileSize);
	offset += DecodeVarint128(value + offset, &fileInformationRecord.lowestHeight);
	offset += DecodeVarint128(value + offset, &fileInformationRecord.highestHeight);
	offset += DecodeVarint128(value + offset, &fileInformationRecord.lowestTimestamp);
	offset += DecodeVarint128(value + offset, &fileInformationRecord.highestTimestamp);
	return fileInformationRecord;
}

uint32_t GetLastBlockFileNumberUsed(const char *value, size_t valLen)
{

}

uint8_t GetIsDatabaseReindexing(const char *value, size_t valLen)
{
	return 0;
}


IndexRecords BuildIndexRecords(char *directory)
{
	IndexRecords			indexRecords;
	indexRecords.blockIndexRecordCount = 0;
	indexRecords.fileInformationRecordCount = 0;

	uint32_t				lastBlockFileNumber;
	uint8_t					isReindexing;

	const char *			key;
	const char *			value;

	size_t keyLen;
	size_t valLen;

	size_t counts[2];
	size_t count = 0;
	LDB_Instance instance = LDB_InitOpen(directory);
	LDB_CountEntriesForPrefixes(instance, "bf", 2, counts);
	indexRecords.blockIndexRecord = malloc(sizeof(BlockIndexRecord) * counts[0]);
	indexRecords.fileInformationRecord = malloc(sizeof(FileInformationRecord) * counts[1]);
	
	LDB_Iterator *iterator = LDB_CreateIterator(instance.db, instance.roptions);
	for (LDB_IterSeekToFirst(iterator); LDB_IterValid(iterator); LDB_IterNext(iterator))
	{
		key = LDB_IterKey(iterator, &keyLen);
		value = LDB_IterValue(iterator, &valLen);
		if (key[0] == 'b')
		{
			BlockIndexRecord blockIndexRecord = GetBlockIndexRecord(key, keyLen, value, valLen);
			indexRecords.blockIndexRecord[blockIndexRecord.height] = blockIndexRecord;
			indexRecords.blockIndexRecordCount++;
		}
	  	else if (key[0] == 'f')
		{
			FileInformationRecord fileInformationRecord = GetFileInformationRecord(key, keyLen, value, valLen);
			//  BUG: indexRecords.fileInformationRecord[fileInformationRecord.filenumber] = fileInformationRecord;
			indexRecords.fileInformationRecordCount++;
		}
	  	else if (key[0] == 'l')
		{
		}
		else if (key[0] == 'R')
		{
		}
		count++;
	}
	LDB_IterDestroy(iterator);
	LDB_Close(instance.db);
	return indexRecords;
}


int ExtractBlockNumber(const char *filename)
{
    const char *blkPtr = strstr(filename, "blk");
    if (blkPtr)
    {
        blkPtr += 3; // Skip past "blk"
        return atoi(blkPtr); // Convert the numeric part to an integer
    }
    return 0;
}

// Comparison function for qsort that compares file numbers
int CompareFileInfo(const void *a, const void *b)
{
    FileInfo *fileA = (FileInfo *)a;
    FileInfo *fileB = (FileInfo *)b;

    int numA = ExtractBlockNumber(fileA->filepath);
    int numB = ExtractBlockNumber(fileB->filepath);

    return (numA - numB); // Sort by numeric value of blk*.dat
}

// Function to sort the FileList based on the number in blk*.dat filenames
void SortFiles(FileList *fileList)
{
    qsort(fileList->files, fileList->count, sizeof(FileInfo), CompareFileInfo);
}


void Indexer(char *datadir)
{
	SET_LOG_PRINT_LINE(0);
	SET_LOG_PRINT_TIME(1);

	char currentDirectory[MAX_PATH_LENGTH];
	char blkDatDir[MAX_PATH_LENGTH];
	char blkIndexesDir[MAX_PATH_LENGTH];
	char chainstateDir[MAX_PATH_LENGTH];
	char coinstatsDir[MAX_PATH_LENGTH];
	char txIndexDir[MAX_PATH_LENGTH];

	char tmpBlkIndexesDir[MAX_PATH_LENGTH];

	SanitizeDirString(datadir);
	// if (!IsDirectory(datadir))
		// GetBitcoinDatadir(datadir);
	// WARN: This won't work on windows.
	// INFO: Might want to use something faster than snprintf
	getcwd(currentDirectory, sizeof(currentDirectory));
    SanitizeDirString(currentDirectory);

	// WARN: This code is unsafe if datadir is of size MAX_PATH_LENGTH
	snprintf(blkDatDir, sizeof(blkDatDir), "%sblocks/", datadir);
    snprintf(blkIndexesDir, sizeof(blkIndexesDir), "%sblocks/index/", datadir);
    snprintf(chainstateDir, sizeof(chainstateDir), "%schainstate/", datadir);
    snprintf(coinstatsDir, sizeof(coinstatsDir), "%sindexes/coinstats/", datadir);
    snprintf(txIndexDir, sizeof(txIndexDir), "%sindexes/txindex/", datadir);

	// WARN: Double Warn , beacuse one is not enough
	snprintf(tmpBlkIndexesDir, sizeof(tmpBlkIndexesDir), "%stmp", currentDirectory);

	//NOTE: We can make a better estimate of the number of blk.dat files 4k is dumb.
	gBlkFiles = ListFiles(blkDatDir, "blk*.dat", 4000);		//NOTE: Init Global Variables -- Array of blk.dat FileInfo
	SortFiles(&gBlkFiles);									//NOTE: We sort that array so that array[0] == blk00000.dat
	LOG_INFO("Copying %s to %s...", blkIndexesDir, tmpBlkIndexesDir);
	CopyDirectory(blkIndexesDir, tmpBlkIndexesDir);
	LOG_INFO("Copying Done.");
	LOG_INFO("Indexing...");
	gIndexRecords = BuildIndexRecords(tmpBlkIndexesDir);		//NOTE: Init Global Variables -- IndexRecords
	LOG_INFO("Indexing Done.");
	DeleteDirectory(tmpBlkIndexesDir);
}



#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif


