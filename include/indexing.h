#ifndef INDEXING_H
 #define INDEXING_H


BlockIndexRecord GetBlockIndexRecord(const char *key, size_t keyLen, const char *value, size_t valLen);
FileInformationRecord GetFileInformationRecord(const char *key, size_t keyLen, const char *value, size_t valLen);
uint32_t GetLastBlockFileNumberUsed(const char *value, size_t valLen);
uint8_t GetIsDatabaseReindexing(const char *value, size_t valLen);
IndexRecords BuildIndexRecords(char *directory);

void Indexer(const char *datadir);

void PrintBlockIndexRecord(const BlockIndexRecord *record);


#endif // !INDEXING_H
