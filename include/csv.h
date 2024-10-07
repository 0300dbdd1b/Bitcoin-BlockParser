
#ifndef CSV_H
#define CSV_H

#include <stdint.h>
#include "structs.h"

/* Function prototypes for converting structures to CSV */
char* BlockHeaderToCSV(BlockHeader* header);
char* InputToCSV(Input* input);
char* OutputToCSV(Output* output);
char* WitnessToCSV(Witness* witness);
char* TransactionToCSV(Transaction* tx);
char* BlockToCSV(Block* block);

/* Helper function for binary to hex string conversion */
char* BinToHex(const uint8_t* bin, size_t len);

#endif /* CSV_H */
