#include "structs.h"
#include "logger.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>

void FreeInput(Input *input)
{
	free(input->scriptSig);
}

void FreeOutput(Output *output)
{
	free(output->scriptPubKey);
}

void FreeWitness(Witness *witness)
{
	for (uint16_t i = 0; i < witness->stackItemsCount; i++)
	{
		free((witness->stackItems[i].item));
	}
	free(witness->stackItems);
}

void FreeTransaction(Transaction *transaction)
{
	for (uint16_t inputIndex = 0; inputIndex < transaction->inputCount; inputIndex++)
		FreeInput(&(transaction->inputs[inputIndex]));
	for (uint16_t outputIndex = 0; outputIndex < transaction->outputCount; outputIndex++)
		FreeOutput(&(transaction->outputs[outputIndex]));

	if (transaction->marker == 0x00 && transaction->flag >= 0x01)
	{
		for (uint16_t witnessIndex = 0; witnessIndex < transaction->inputCount; witnessIndex++)
		{
			FreeWitness(&(transaction->witnesses[witnessIndex]));
		}
	}
}

void FreeBlock(Block *block)
{
	if (block->transactions)
	{
		for (uint16_t i = 0; i < block->txCount; i++)
			FreeTransaction(&(block->transactions[i]));
		free(block->transactions);
	}
}


void FreeFileList(FileList *fileList)
{
    for (size_t i = 0; i < fileList->count; i++)
    {
        if (fileList->files[i].isOpen)
        {
            fclose(fileList->files[i].file);
        }
        free(fileList->files[i].filepath);
    }

    free(fileList->files);
    fileList->files = NULL;
    fileList->count = 0;
    fileList->totalSize = 0;
}
