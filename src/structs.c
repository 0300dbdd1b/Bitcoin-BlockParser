#include "structs.h"
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
	if (witness->stackItems)
	{
		for (uint8_t i = 0; i < witness->stackItemsCount; i++)
		{
			free(witness->stackItems[i].item);
		}
		free(witness->stackItems);
	}
}

void FreeTransaction(Transaction *transaction)
{

}

void FreeBlock(Block *block)
{
	if (block->transactions)
	{
		for (uint8_t i = 0; i < block->txCount; i++)
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
