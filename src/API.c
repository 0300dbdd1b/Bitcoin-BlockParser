#include "structs.h"
#include "blkdat.h"
#include <stdlib.h>

extern IndexRecords gIndexRecords;
extern FileList gBlkFiles;
static OpenFileCache openFiles[MAX_OPEN_FILES];


Block GetBlock(int height)
{
	Block block;
	block = ReadBlkDatFile(&gBlkFiles.files[gIndexRecords.blockIndexRecord[height].blockFile], gIndexRecords.blockIndexRecord[height].blockOffset);
	block.height = height;
	return block;
}

Block *GetBlocks(int height, int count)
{
	Block *blocks = malloc(sizeof(Block) * count);
	for (int i = 0; i < count; i++)
	{
		blocks[i] = GetBlock(height + i);
	}
	return blocks;
}

BlockHeader GetBlockHeader(int height)
{
	//NOTE: We get the blockheader from the leveldb index as it is faster. Might want to add a fallback in case leveldb is not available.
	return gIndexRecords.blockIndexRecord[height].blockHeader;
}

BlockStats GetBlockStats(int height)
{
	Block block;
	BlockStats blockStats;

	block = GetBlock(height);

	return blockStats;
}
