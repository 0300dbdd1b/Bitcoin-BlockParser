#ifndef LEVELDB_H
 #define LEVELDB_H
 #include <leveldb/c.h>


// SEE: https://github.com/google/leveldb/blob/main/include/leveldb/c.h
/*
  Copyright (c) 2011 The LevelDB Authors. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be
  found in the LICENSE file. See the AUTHORS file for names of contributors.

  C bindings for leveldb.  May be useful as a stable ABI that can be
  used by programs that keep leveldb in a shared library, or for
  a JNI api.

  Does not support:
  . getters for the option types
  . custom comparators that implement key shortening
  . custom iter, db, env, cache implementations using just the C bindings

  Some conventions:

  (1) We expose just opaque struct pointers and functions to clients.
  This allows us to change internal representations without having to
  recompile clients.

  (2) For simplicity, there is no equivalent to the Slice type.  Instead,
  the caller has to pass the pointer and length as separate
  arguments.

  (3) Errors are represented by a null-terminated c string.  NULL
  means no error.  All operations that can raise an error are passed
  a "char** errptr" as the last argument.  One of the following must
  be true on entry:
     *errptr == NULL
     *errptr points to a malloc()ed null-terminated error message
       (On Windows, *errptr must have been malloc()-ed by this library.)
  On success, a leveldb routine leaves *errptr unchanged.
  On failure, leveldb frees the old value of *errptr and
  set *errptr to a malloc()ed error message.

  (4) Bools have the type uint8_t (0 == false; rest == true)

  (5) All of the pointer arguments must be non-NULL.
*/


/* DB operations */
#define LDB_Open             leveldb_open
#define LDB_Close            leveldb_close
#define LDB_Put              leveldb_put
#define LDB_Delete           leveldb_delete
#define LDB_Write            leveldb_write
#define LDB_Get              leveldb_get
#define LDB_CreateIterator   leveldb_create_iterator
#define LDB_CreateSnapshot   leveldb_create_snapshot
#define LDB_ReleaseSnapshot  leveldb_release_snapshot
#define LDB_PropertyValue    leveldb_property_value
#define LDB_ApproximateSizes leveldb_approximate_sizes
#define LDB_CompactRange     leveldb_compact_range

/* Management operations */
#define LDB_DestroyDB  leveldb_destroy_db
#define LDB_RepairDB   leveldb_repair_db

/* Iterator */
#define LDB_IterDestroy     leveldb_iter_destroy
#define LDB_IterValid       leveldb_iter_valid
#define LDB_IterSeekToFirst leveldb_iter_seek_to_first
#define LDB_IterSeekToLast  leveldb_iter_seek_to_last
#define LDB_IterSeek        leveldb_iter_seek
#define LDB_IterNext        leveldb_iter_next
#define LDB_IterPrev        leveldb_iter_prev
#define LDB_IterKey         leveldb_iter_key
#define LDB_IterValue       leveldb_iter_value
#define LDB_IterGetError    leveldb_iter_get_error

/* Write batch */
#define LDB_WriteBatchCreate   leveldb_writebatch_create
#define LDB_WriteBatchDestroy  leveldb_writebatch_destroy
#define LDB_WriteBatchClear    leveldb_writebatch_clear
#define LDB_WriteBatchPut      leveldb_writebatch_put
#define LDB_WriteBatchDelete   leveldb_writebatch_delete
#define LDB_WriteBatchIterate  leveldb_writebatch_iterate
#define LDB_WriteBatchAppend   leveldb_writebatch_append

/* Options */
#define LDB_OptionsCreate                     leveldb_options_create
#define LDB_OptionsDestroy                    leveldb_options_destroy
#define LDB_OptionsSetComparator              leveldb_options_set_comparator
#define LDB_OptionsSetFilterPolicy            leveldb_options_set_filter_policy
#define LDB_OptionsSetCreateIfMissing         leveldb_options_set_create_if_missing
#define LDB_OptionsSetErrorIfExists           leveldb_options_set_error_if_exists
#define LDB_OptionsSetParanoidChecks          leveldb_options_set_paranoid_checks
#define LDB_OptionsSetEnv                     leveldb_options_set_env
#define LDB_OptionsSetInfoLog                 leveldb_options_set_info_log
#define LDB_OptionsSetWriteBufferSize         leveldb_options_set_write_buffer_size
#define LDB_OptionsSetMaxOpenFiles            leveldb_options_set_max_open_files
#define LDB_OptionsSetCache                   leveldb_options_set_cache
#define LDB_OptionsSetBlockSize               leveldb_options_set_block_size
#define LDB_OptionsSetBlockRestartInterval    leveldb_options_set_block_restart_interval
#define LDB_OptionsSetMaxFileSize             leveldb_options_set_max_file_size
#define LDB_OptionsSetCompression             leveldb_options_set_compression

/* Comparator */
#define LDB_ComparatorCreate  leveldb_comparator_create
#define LDB_ComparatorDestroy leveldb_comparator_destroy

/* Filter policy */
#define LDB_FilterPolicyCreate        leveldb_filterpolicy_create
#define LDB_FilterPolicyDestroy       leveldb_filterpolicy_destroy
#define LDB_FilterPolicyCreateBloom   leveldb_filterpolicy_create_bloom

/* Read options */
#define LDB_ReadOptionsCreate               leveldb_readoptions_create
#define LDB_ReadOptionsDestroy              leveldb_readoptions_destroy
#define LDB_ReadOptionsSetVerifyChecksums   leveldb_readoptions_set_verify_checksums
#define LDB_ReadOptionsSetFillCache         leveldb_readoptions_set_fill_cache
#define LDB_ReadOptionsSetSnapshot          leveldb_readoptions_set_snapshot

/* Write options */
#define LDB_WriteOptionsCreate   leveldb_writeoptions_create
#define LDB_WriteOptionsDestroy  leveldb_writeoptions_destroy
#define LDB_WriteOptionsSetSync  leveldb_writeoptions_set_sync

/* Cache */
#define LDB_CacheCreateLRU   leveldb_cache_create_lru
#define LDB_CacheDestroy     leveldb_cache_destroy

/* Env */
#define LDB_CreateDefaultEnv        leveldb_create_default_env
#define LDB_EnvDestroy              leveldb_env_destroy
#define LDB_EnvGetTestDirectory     leveldb_env_get_test_directory

/* Utility */
#define LDB_Free                 leveldb_free
#define LDB_MajorVersion         leveldb_major_version
#define LDB_MinorVersion         leveldb_minor_version

typedef struct LDB_Instance
{
	const char *dbPath;
	leveldb_t *db;
	leveldb_options_t *options;
	leveldb_readoptions_t *roptions;
	leveldb_writeoptions_t *woptions;
	char *errors;
	size_t errorCount;
} LDB_Instance;

typedef  leveldb_t				LDB_DB;
typedef  leveldb_cache_t		LDB_Cache;
typedef  leveldb_comparator_t 	LDB_Comparator;
typedef  leveldb_env_t 			LDB_Env;
typedef  leveldb_filelock_t 	LDB_FileLock;
typedef  leveldb_filterpolicy_t LDB_FilterPolicy;
typedef  leveldb_iterator_t 	LDB_Iterator;
typedef  leveldb_logger_t 		LDB_Logger;
typedef  leveldb_options_t 		LDB_Options;
typedef  leveldb_randomfile_t 	LDB_RandomFile;
typedef  leveldb_readoptions_t 	LDB_ReadOptions;
typedef  leveldb_seqfile_t 		LDB_SeqFile;
typedef  leveldb_snapshot_t 	LDB_Snapshot;
typedef  leveldb_writablefile_t LDB_WritableFile;
typedef  leveldb_writebatch_t 	LDB_WriteBatch;
typedef  leveldb_writeoptions_t LDB_WriteOptions;


LDB_Instance LDB_Init(const char *path);
LDB_Instance LDB_InitOpen(const char *path);
size_t LDB_CountEntries(const LDB_Instance instance, const char keyPrefix);
void LDB_CountEntriesForPrefixes(LDB_Instance instance, const char* keyPrefixes, size_t prefixCount, size_t* counts);

#endif // !LEVELDB_H
