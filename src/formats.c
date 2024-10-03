
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "structs.h"
#include "utils.h"

char* Uint8ToHex(const uint8_t* data, size_t length)
{
    char* hexString = (char*)malloc(length * 2 + 1);
    if (hexString == NULL) {
        return NULL;  // Return NULL if memory allocation fails
    }

    for (size_t i = 0; i < length; ++i) {
        snprintf(hexString + i * 2, 3, "%02x", data[i]);
    }

    hexString[length * 2] = '\0';  // Null-terminate the string
    return hexString;
}

cJSON* VinToJson(Input *input)
{
    cJSON *jsonVin = cJSON_CreateObject();

    char* txidHex = Uint8ToHex(input->txid, 32);
    if (txidHex) {
        cJSON_AddStringToObject(jsonVin, "txid", txidHex);
        free(txidHex);
    }
    cJSON_AddNumberToObject(jsonVin, "vout", input->vout);

    if (input->scriptSigSize > 0 && input->scriptSig != NULL)
    {
        cJSON *jsonScriptSig = cJSON_CreateObject();
        char* scriptSigHex = Uint8ToHex(input->scriptSig, input->scriptSigSize);
        if (scriptSigHex) {
            cJSON_AddStringToObject(jsonScriptSig, "hex", scriptSigHex);
            free(scriptSigHex);
        }
        cJSON_AddStringToObject(jsonScriptSig, "asm", "");  // Placeholder for asm parsing
        cJSON_AddItemToObject(jsonVin, "scriptSig", jsonScriptSig);
    }

    cJSON_AddNumberToObject(jsonVin, "sequence", input->sequence);

    return jsonVin;
}

cJSON* VoutToJson(Output *output, int index)
{
    cJSON *jsonVout = cJSON_CreateObject();
    double valueBtc = output->amount / 100000000.0;
    cJSON_AddNumberToObject(jsonVout, "value", valueBtc);
    cJSON_AddNumberToObject(jsonVout, "n", index);

    if (output->scriptPubKeySize > 0 && output->scriptPubKey != NULL)
    {
        cJSON *jsonScriptPubKey = cJSON_CreateObject();
        char* scriptPubKeyHex = Uint8ToHex(output->scriptPubKey, output->scriptPubKeySize);
        if (scriptPubKeyHex) {
            cJSON_AddStringToObject(jsonScriptPubKey, "hex", scriptPubKeyHex);
            free(scriptPubKeyHex);
        }
        cJSON_AddStringToObject(jsonScriptPubKey, "asm", "");  // Placeholder for asm parsing
        cJSON_AddStringToObject(jsonScriptPubKey, "type", "pubkeyhash");  // Example type
        cJSON_AddItemToObject(jsonVout, "scriptPubKey", jsonScriptPubKey);
    }

    return jsonVout;
}

cJSON* TxToJson(Transaction *tx)
{
    cJSON *jsonTx = cJSON_CreateObject();

    char* txidHex = Uint8ToHex(tx->txid, 32);
    char* wtxidHex = Uint8ToHex(tx->wtxid, 32);
    if (txidHex) {
        cJSON_AddStringToObject(jsonTx, "txid", txidHex);
        free(txidHex);
    }
    if (wtxidHex) {
        cJSON_AddStringToObject(jsonTx, "wtxid", wtxidHex);
        free(wtxidHex);
    }

    cJSON_AddNumberToObject(jsonTx, "size", tx->size);
    cJSON_AddNumberToObject(jsonTx, "vsize", tx->size);  // Placeholder for real calculation
    cJSON_AddNumberToObject(jsonTx, "weight", tx->size * 4);  // Placeholder

    cJSON_AddNumberToObject(jsonTx, "version", tx->version);
    cJSON_AddNumberToObject(jsonTx, "locktime", tx->locktime);

    cJSON *vinArray = cJSON_CreateArray();
    for (uint16_t i = 0; i < tx->inputCount; ++i)
    {
        cJSON_AddItemToArray(vinArray, VinToJson(&tx->inputs[i]));
    }
    cJSON_AddItemToObject(jsonTx, "vin", vinArray);

    cJSON *voutArray = cJSON_CreateArray();
    for (uint16_t i = 0; i < tx->outputCount; ++i)
    {
        cJSON_AddItemToArray(voutArray, VoutToJson(&tx->outputs[i], i));
    }
    cJSON_AddItemToObject(jsonTx, "vout", voutArray);

    return jsonTx;
}

cJSON* BlockToJson(Block *block)
{
    cJSON *jsonBlock = cJSON_CreateObject();

	cJSON_AddNumberToObject(jsonBlock, "height", block->height);
    char* hashHex = Uint8ToHex(block->hash, 32);
    if (hashHex) {
        cJSON_AddStringToObject(jsonBlock, "hash", hashHex);
        free(hashHex);
    }
    cJSON_AddNumberToObject(jsonBlock, "size", block->blockSize);
    cJSON_AddNumberToObject(jsonBlock, "nTx", block->txCount);
    cJSON_AddNumberToObject(jsonBlock, "version", block->header.version);

    char* prevBlockHashHex = Uint8ToHex(block->header.prevBlockHash, 32);
    if (prevBlockHashHex)
	{
        cJSON_AddStringToObject(jsonBlock, "previousblockhash", prevBlockHashHex);
        free(prevBlockHashHex);
    }

    char* merkleRootHex = Uint8ToHex(block->header.merkleRoot, 32);
    if (merkleRootHex)
	{
        cJSON_AddStringToObject(jsonBlock, "merkleroot", merkleRootHex);
        free(merkleRootHex);
    }

    cJSON_AddNumberToObject(jsonBlock, "time", block->header.time);
    cJSON_AddNumberToObject(jsonBlock, "nonce", block->header.nonce);

    char bitsHex[9];  // 8 characters + null terminator for 4-byte value in hex
    snprintf(bitsHex, sizeof(bitsHex), "%08x", block->header.bits);
    cJSON_AddStringToObject(jsonBlock, "bits", bitsHex);

    cJSON *txArray = cJSON_CreateArray();
    for (uint16_t i = 0; i < block->txCount; ++i)
    {
        cJSON_AddItemToArray(txArray, TxToJson(&block->transactions[i]));
    }
    cJSON_AddItemToObject(jsonBlock, "tx", txArray);

    return jsonBlock;
}

cJSON* BlocksToJson(Block *blocks, size_t blockCount)
{
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *jsonBlocks = cJSON_CreateArray();

    for (size_t i = 0; i < blockCount; i++)
    {
        cJSON_AddItemToArray(jsonBlocks, BlockToJson(&blocks[i]));
    }

    cJSON_AddItemToObject(jsonObject, "blocks", jsonBlocks);

    return jsonObject;
}

