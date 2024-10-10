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


// WARN: UNSAFE FUNCTION -- can bloat memory
Blocks GetBlocks(int height, int count)
{
	Blocks blocks;
	blocks.count = 0;
	blocks.blocks = malloc(sizeof(Block) * count);
	for (int i = 0; i < count; i++)
	{
		blocks.blocks[i] = GetBlock(height + i);
		blocks.count++;
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
