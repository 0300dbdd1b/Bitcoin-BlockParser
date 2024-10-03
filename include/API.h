#ifndef API_H
 #define API_H
 #include "structs.h"
 #include "cJSON.h"

char* Uint8ToHex(const uint8_t* data, size_t length);
cJSON* VinToJson(Input *input);
cJSON* VoutToJson(Output *output, int index);
cJSON* TxToJson(Transaction *tx);
cJSON* BlockToJson(Block *block);
cJSON* BlocksToJson(Block *blocks, size_t blockCount);


Block GetBlock(int height);
Block *GetBlocks(int height, int count);
BlockHeader GetBlockHeader(int height);
BlockStats GetBlockStats(int height);

#endif // !API_H
