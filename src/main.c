#include "blkdat.h"
#include "platform.h"
#include "sha256.h"
#include "structs.h"
#include "utils.h"
#include "debug.h"
#include "leveldb.h"
#include "parser.h"
#include "cJSON.h"
#include "API.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#include <limits.h>
#include "indexing.h"
const char *__asan_default_options() { return "detect_leaks=0"; }

extern FileList		gBlkFiles;
extern IndexRecords gIndexRecords;
FileList gBlkFiles;
IndexRecords gIndexRecords;



int main(int ac, char *av[])
{
	SET_LOG_LEVEL(LOG_LEVEL_DEBUG);
	SET_LOG_MODE(LOG_MODE_STDERR);
    TIME_BLOCK_START(main);  // Start benchmarking total execution time
    char directory[MAX_PATH_LENGTH];

    // Check if program is running as sudo
    if (getuid() != 0)
        LOG_WARN("Must be run as sudo");

    // Determine the directory to use for block data
    if (ac < 2)
    {

		LOG_INFO("Looking for data at %s", DEFAULT_BITCOIN_DIR);
        if (IsDirectory(DEFAULT_BITCOIN_DIR))
            strcpy(directory, DEFAULT_BITCOIN_DIR);
    }
    else
    {
        memcpy(directory, av[1], strlen(av[1]));
        directory[strlen(av[1])] = '\0';
    }

    SanitizeDirString(directory);
    if (!IsDirectory(directory))
    {
        if (IsDirectory(DEFAULT_BITCOIN_BLOCKS_INDEX_DIR))
            strcpy(directory, DEFAULT_BITCOIN_BLOCKS_INDEX_DIR);
        else
        {
			LOG_FATAL("%s is not a directory", directory);
            return 1;
        }
    }

    // Index the directory (assume Indexer populates gIndexRecords)
    Indexer(directory);
    TIME_BLOCK_START(loadBlocks);

    // Load blocks
    Block *blocks = GetBlocks(600000, 144);
    
    // Convert blocks to JSON
	TIME_BLOCK_START(jsonconvert);
    cJSON *json = BlocksToJson(blocks, 144);
    char *json_string = cJSON_Print(json);
	TIME_BLOCK_END(jsonconvert, "Json convertion took");

    // Write the JSON string to a file
    FILE *file = fopen("output.json", "w");
    if (file == NULL) {
        perror("Failed to open file for writing");
        cJSON_Delete(json);
        free(json_string);
        return 1;
    }

    fwrite(json_string, sizeof(char), strlen(json_string), file);
    fclose(file);

    // Clean up
    cJSON_Delete(json);
    free(json_string);

    TIME_BLOCK_END(loadBlocks, "Total time to load all blocks: ");  // End benchmarking block loading time

    // Free resources before exiting
    FreeFileList(&gBlkFiles);
    TIME_BLOCK_END(main, "Total execution time: ");  // End benchmarking total execution time
    
    return 0;
}
