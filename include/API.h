#ifndef API_H
 #define API_H
 #include "structs.h"

char* Uint8ToHex(const uint8_t* data, size_t length);


Block GetBlock(int height);
Block *GetBlocks(int height, int count);
BlockHeader GetBlockHeader(int height);
BlockStats GetBlockStats(int height);

#endif // !API_H
