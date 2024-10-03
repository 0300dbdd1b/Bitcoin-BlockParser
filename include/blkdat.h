#ifndef BLKDAT_H
 #define BLKDAT_H
 #include "structs.h"
void InitBlockHeader(BlockHeader *header);
void InitInput(Input *input);
void InitOutput(Output *output);
void InitStackItem(StackItem *item);
void InitWitness(Witness *witness);
void InitTransaction(Transaction *tx);
void InitBlock(Block *block);

Block ReadBlkDatFile(FileInfo *file, size_t offset);
#endif // !BLKDAT_H
