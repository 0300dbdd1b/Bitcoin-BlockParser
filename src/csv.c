
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "structs.h" // Assuming the structures Block, Transaction, Input, Output, etc., are defined here.

#define HASH_STR_LEN 65
#define MAX_SATOSHI_LENGTH 21  // To represent BTC value in satoshis (including null terminator)

// Function to convert a hash to a dynamically allocated string
char *BlockHashToString(uint8_t *hash) {
    static char hashStr[HASH_STR_LEN];
    for (int i = 0; i < 32; i++) {
        snprintf(hashStr + (i * 2), HASH_STR_LEN - (i * 2), "%02x", hash[i]);
    }
    return hashStr;
}

// Function to convert a transaction hash to a string
char *TransactionHashToString(uint8_t *txid) {
    static char txidStr[HASH_STR_LEN];
    for (int i = 0; i < 32; i++) {
        snprintf(txidStr + (i * 2), HASH_STR_LEN - (i * 2), "%02x", txid[i]);
    }
    return txidStr;
}

// Function to convert a script to a dynamically allocated string
char *ScriptToString(uint8_t *script, uint16_t scriptSize) {
    // Each byte takes 2 characters in hex + 1 for null terminator
    uint16_t hexStrSize = (scriptSize * 2) + 1;
    char *scriptStr = (char *)malloc(hexStrSize);
    
    if (scriptStr == NULL) {
        fprintf(stderr, "Failed to allocate memory for scriptStr\n");
        return NULL;
    }
    
    for (int i = 0; i < scriptSize; i++) {
        snprintf(scriptStr + (i * 2), hexStrSize - (i * 2), "%02x", script[i]);
    }
    
    return scriptStr;
}

// Function to encode block-level data to CSV
void EncodeBlockToCSV(FILE *file, Block *block) {
    fprintf(file, "%llu,%s,%u,%s,%s,%u,%u,%u,%u,%u\n",
            block->height,
            BlockHashToString(block->hash),       // Convert hash bytes to string
            block->header.version,
            BlockHashToString(block->header.prevBlockHash),
            BlockHashToString(block->header.merkleRoot),
            block->header.time,
            block->header.bits,
            block->header.nonce,
            block->txCount,
            block->blockSize);
}

// Function to encode transaction-level data to CSV
void EncodeTransactionToCSV(FILE *file, Block *block, Transaction *tx) {
    for (int i = 0; i < tx->inputCount; i++) {
        Input *input = &tx->inputs[i];
        char *scriptSigStr = ScriptToString(input->scriptSig, input->scriptSigSize);
        
        if (scriptSigStr == NULL) {
            // Handle memory allocation error
            return;
        }

        fprintf(file, "%llu,%s,%u,%u,%u,%u,%u,%s,%u,\"%s\",%u,%d,%f,\"%s\",\"%s\"\n",
                block->height,
                TransactionHashToString(tx->txid), // Convert txid to string
                tx->version,
                tx->inputCount,
                tx->outputCount,
                tx->locktime,
                i,
                TransactionHashToString(input->txid),
                input->vout,
                scriptSigStr,   // Use the dynamically allocated script string
                input->sequence,
                -1,             // Output-related fields remain empty for inputs
                0.0,
                "",
                "");            // No scriptPubKey or witness for inputs

        free(scriptSigStr);  // Free the allocated memory
    }

    for (int i = 0; i < tx->outputCount; i++) {
        Output *output = &tx->outputs[i];
        char *scriptPubKeyStr = ScriptToString(output->scriptPubKey, output->scriptPubKeySize);
        
        if (scriptPubKeyStr == NULL) {
            // Handle memory allocation error
            return;
        }

        fprintf(file, "%llu,%s,%u,%u,%u,%u,%d,%s,%d,\"%s\",%u,%d,%f,\"%s\",\"\"\n",
                block->height,
                TransactionHashToString(tx->txid), // Convert txid to string
                tx->version,
                tx->inputCount,
                tx->outputCount,
                tx->locktime,
                -1,             // Input-related fields remain empty for outputs
                "",
                0,
                "",
                0,
                i,
                (double)output->amount / 100000000.0, // Convert satoshis to BTC
                scriptPubKeyStr,  // Use the dynamically allocated script string
                ""); // Witness data would go here if available

        free(scriptPubKeyStr);  // Free the allocated memory
    }
}

