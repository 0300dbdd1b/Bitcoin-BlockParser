#ifndef STRUCTS_H
 #define STRUCTS_H
#include <stdint.h>
 #include <stdio.h>
 #include <stddef.h>

 #define BLOCK_0_TIMESTAMP 1231006505		// 03 Jan 2009, 18:15:05
 #define BLOCK_0_HASH 0x000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f

 #define BLOCK_210000_TIMESTAMP 1354116278 	// 28 Nov 2012, 15:24:38
 #define BLOCK_210000_HASH 0x000000000000048b95347e83192f69cf0366076336c639f9b7228e9ba171342e

 #define BLOCK_420000_TIMESTAMP 1468082773	// 09 Jul 2016, 16:46:13
 #define BLOCK_420000_HASH 0x000000000000000002cce816c0ab2c5c269cb081896b7dcb34b8422d6b74ffa1

 #define BLOCK_630000_TIMESTAMP 1589225023	// 11 May 2020, 19:23:43
 #define BLOCK_630000_HASH 0x000000000000000000024bead8df69990852c202db0e0097c1a12ea637d7e96d

 #define BLOCK_840000_TIMESTAMP 1713571767	// 20 Apr 2024, 00:09:27
 #define BLOCK_840000_HASH 0x0000000000000000000320283a032748cef8227873ff4872689bf23f1cda83a5

 #define MAGIC_BYTES_MAINNET	0xf9beb4d9
 #define MAGIC_BYTES_TESTNET3	0x0b110907
 #define MAGIC_BYTES_TESTNET4	0x1c163f28
 #define MAGIC_BYTES_REGTEST	0xfabfb5da

 #define TARGET_BLOCK_INTERVAL (2016/2015) * 60 * 10 // take into account the off-by-one bug


 // SEE: https://github.com/bitcoin/bitcoin/blob/6d546336e800f7b8990fececab6bc08413f28690/src/node/blockstorage.h#L70
 #define MAX_BLOCKFILE_CHUNK_SIZE	0x100000		// 16 MiB
 // SEE: https://github.com/bitcoin/bitcoin/blob/6d546336e800f7b8990fececab6bc08413f28690/src/node/blockstorage.h#L72
 #define MAX_UNDOFILE_CHUNK_SIZE	0x100000		// 1 MiB
 // SEE: https://github.com/bitcoin/bitcoin/blob/6d546336e800f7b8990fececab6bc08413f28690/src/node/blockstorage.h#L74
 #define MAX_BLOCKFILE_SIZE 		0x8000000 		// 128 MiB

 #define SHA256_HASH_SIZE  32	// 32 bytes
 #define BLOCKHEADER_SIZE  80	// 80 bytes

 #define MAX_BLOCK_SIZE 1000000
 #define MAX_SCRIPT_ELEMENT_SIZE 520		// Maximum number of bytes pushable to the stack
 #define MAX_OPS_PER_SCRIPT 201				// Maximum number of non-push operations per script
 #define MAX_PUBKEYS_PER_MULTISIG 20		// Maximum number of public keys per multisig
 #define MAX_PUBKEYS_PER_MULTI_A 999		// The limit of keys in OP_CHECKSIGADD-based scripts. It is due to the stack limit in BIP342.
 #define MAX_SCRIPT_SIZE 10000				// Maximum script length in bytes
 #define MAX_STACK_SIZE 1000				// Maximum number of values on script interpreter stack
// Threshold for nLockTime: below this value it is interpreted as block number,
// otherwise as UNIX timestamp.
 #define LOCKTIME_THRESHOLD 500000000		// Tue Nov  5 00:53:20 1985 UTC
// Maximum nLockTime. Since a lock time indicates the last invalid timestamp, a
// transaction with this lock time will never be valid unless lock time
// checking is disabled (by setting all input sequence numbers to
// SEQUENCE_FINAL).
 #define LOCKTIME_MAX 0xFFFFFFFFU

// Tag for input annex. If there are at least two witness elements for a transaction input,
// and the first byte of the last element is 0x50, this last element is called annex, and
// has meanings independent of the script
 #define ANNEX_TAG 0x50
// Validation weight per passing signature (Tapscript only, see BIP 342).
 #define VALIDATION_WEIGHT_PER_SIGOP_PASSED 50
// How much weight budget is added to the witness size (Tapscript only, see BIP 342).
 #define VALIDATION_WEIGHT_OFFSET 50


 #define MAX_COMPACT_SIZE_BYTES 10
 #define MAX_VARINT128_BYTES 10
 
 #define MAX_OPEN_FILES 5
 #if MAX_OPEN_FILES > FOPEN_MAX
 	#error MAX_OPEN_FILES > FOPEN_MAX
 #endif

typedef struct FileInfo
{
	char		*filepath;
	uint8_t		isOpen;
	FILE		*file;
} FileInfo;

typedef struct FileList
{
	FileInfo *files;
	size_t	count;
	size_t totalSize;
} FileList;

typedef struct OpenFileCache
{
    FileInfo *fileInfo;      // Pointer to the FileInfo of the open file
    uint32_t lastAccessTime; // Last access time to implement LRU
} OpenFileCache;

// SEE: https://github.com/bitcoin/bitcoin/blob/a05987defdd2c7400f6057953535f76aeb52fa28/src/script/script.h#L72
typedef enum OpcodeType
{
    // push value
    OP_0 = 0x00,
    OP_FALSE = OP_0,
    OP_PUSHDATA1 = 0x4c,
    OP_PUSHDATA2 = 0x4d,
    OP_PUSHDATA4 = 0x4e,
    OP_1NEGATE = 0x4f,
    OP_RESERVED = 0x50,
    OP_1 = 0x51,
    OP_TRUE=OP_1,
    OP_2 = 0x52,
    OP_3 = 0x53,
    OP_4 = 0x54,
    OP_5 = 0x55,
    OP_6 = 0x56,
    OP_7 = 0x57,
    OP_8 = 0x58,
    OP_9 = 0x59,
    OP_10 = 0x5a,
    OP_11 = 0x5b,
    OP_12 = 0x5c,
    OP_13 = 0x5d,
    OP_14 = 0x5e,
    OP_15 = 0x5f,
    OP_16 = 0x60,

    // control
    OP_NOP = 0x61,
    OP_VER = 0x62,
    OP_IF = 0x63,
    OP_NOTIF = 0x64,
    OP_VERIF = 0x65,
    OP_VERNOTIF = 0x66,
    OP_ELSE = 0x67,
    OP_ENDIF = 0x68,
    OP_VERIFY = 0x69,
    OP_RETURN = 0x6a,

    // stack ops
    OP_TOALTSTACK = 0x6b,
    OP_FROMALTSTACK = 0x6c,
    OP_2DROP = 0x6d,
    OP_2DUP = 0x6e,
    OP_3DUP = 0x6f,
    OP_2OVER = 0x70,
    OP_2ROT = 0x71,
    OP_2SWAP = 0x72,
    OP_IFDUP = 0x73,
    OP_DEPTH = 0x74,
    OP_DROP = 0x75,
    OP_DUP = 0x76,
    OP_NIP = 0x77,
    OP_OVER = 0x78,
    OP_PICK = 0x79,
    OP_ROLL = 0x7a,
    OP_ROT = 0x7b,
    OP_SWAP = 0x7c,
    OP_TUCK = 0x7d,

    // splice ops
    OP_CAT = 0x7e,
    OP_SUBSTR = 0x7f,
    OP_LEFT = 0x80,
    OP_RIGHT = 0x81,
    OP_SIZE = 0x82,

    // bit logic
    OP_INVERT = 0x83,
    OP_AND = 0x84,
    OP_OR = 0x85,
    OP_XOR = 0x86,
    OP_EQUAL = 0x87,
    OP_EQUALVERIFY = 0x88,
    OP_RESERVED1 = 0x89,
    OP_RESERVED2 = 0x8a,

    // numeric
    OP_1ADD = 0x8b,
    OP_1SUB = 0x8c,
    OP_2MUL = 0x8d,
    OP_2DIV = 0x8e,
    OP_NEGATE = 0x8f,
    OP_ABS = 0x90,
    OP_NOT = 0x91,
    OP_0NOTEQUAL = 0x92,

    OP_ADD = 0x93,
    OP_SUB = 0x94,
    OP_MUL = 0x95,
    OP_DIV = 0x96,
    OP_MOD = 0x97,
    OP_LSHIFT = 0x98,
    OP_RSHIFT = 0x99,

    OP_BOOLAND = 0x9a,
    OP_BOOLOR = 0x9b,
    OP_NUMEQUAL = 0x9c,
    OP_NUMEQUALVERIFY = 0x9d,
    OP_NUMNOTEQUAL = 0x9e,
    OP_LESSTHAN = 0x9f,
    OP_GREATERTHAN = 0xa0,
    OP_LESSTHANOREQUAL = 0xa1,
    OP_GREATERTHANOREQUAL = 0xa2,
    OP_MIN = 0xa3,
    OP_MAX = 0xa4,

    OP_WITHIN = 0xa5,

    // crypto
    OP_RIPEMD160 = 0xa6,
    OP_SHA1 = 0xa7,
    OP_SHA256 = 0xa8,
    OP_HASH160 = 0xa9,
    OP_HASH256 = 0xaa,
    OP_CODESEPARATOR = 0xab,
    OP_CHECKSIG = 0xac,
    OP_CHECKSIGVERIFY = 0xad,
    OP_CHECKMULTISIG = 0xae,
    OP_CHECKMULTISIGVERIFY = 0xaf,

    // expansion
    OP_NOP1 = 0xb0,
    OP_CHECKLOCKTIMEVERIFY = 0xb1,
    OP_NOP2 = OP_CHECKLOCKTIMEVERIFY,
    OP_CHECKSEQUENCEVERIFY = 0xb2,
    OP_NOP3 = OP_CHECKSEQUENCEVERIFY,
    OP_NOP4 = 0xb3,
    OP_NOP5 = 0xb4,
    OP_NOP6 = 0xb5,
    OP_NOP7 = 0xb6,
    OP_NOP8 = 0xb7,
    OP_NOP9 = 0xb8,
    OP_NOP10 = 0xb9,

    // Opcode added by BIP 342 (Tapscript)
    OP_CHECKSIGADD = 0xba,

    OP_INVALIDOPCODE = 0xff,
} OpcodeType;

 // SEE: https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/primitives/block.h#L21
typedef struct BlockHeader
{
	uint8_t 	version;			// 4 bytes - Block Version
	uint8_t 	prevBlockHash[32];	// 32 bytes - Previous Block Hash
	uint8_t 	merkleRoot[32];		// 32 bytes - Merkle Root of all the transactions
	uint32_t 	time;				// 4 bytes - UNIX timestamp
	uint32_t 	bits;				// 4 bytes - Target
	uint32_t 	nonce;				// 4 bytes - Nonce
}BlockHeader;						// 80 bytes

// SEE: https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/primitives/transaction.h#L66
typedef struct Input
{
    uint8_t		txid[32];			// 32 bytes - Natural byte order
    uint32_t	vout;				// 4 bytes - Little-endian
    uint16_t	scriptSigSize;		// Variable size - Compact size
    uint8_t		*scriptSig;			// Variable size - Script (unlocking code)
    uint32_t	sequence;			// 4 bytes - Little-endian
} Input;

// SEE: https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/primitives/transaction.h#L149
typedef struct Output
{
    uint64_t	amount;				// 8 bytes - Little-endian (value in satoshis)
    uint16_t	scriptPubKeySize;	// 2 bytes - Compact size
    uint8_t		*scriptPubKey;		// Variable size - Script (locking code)
} Output;

typedef struct StackItem
{
        uint16_t size;           // Variable size - Compact size
        uint8_t *item;           // Variable size - Bytes (data to push onto stack)
} StackItem;

// SEE: https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/script/script.h#L573
typedef struct Witness
{
    uint16_t stackItemsCount;	// 2 bytes
    StackItem *stackItems;  	// Array of stack items
} Witness;

// SEE: https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/primitives/transaction.h#L295
typedef struct Transaction
{
	uint32_t	version;			// 4 bytes - little-endian
	uint8_t		marker;				// 1 byte - segwit marker
	uint8_t		flag;				// 1 byte - segwit flag
	uint16_t	inputCount;		//  2 bytes - compact size
	Input		*inputs;			// Array of inputs
	uint16_t 	outputCount;		// Variable size - compact size
	Output 		*outputs;			// Array of outputs
	Witness		*witnesses;			// Array of witness data (corresponding to inputs)
	uint32_t	locktime;			// 4 bytes - little-endian
	uint16_t	size;
	uint8_t		txid[32];
	uint8_t		wtxid[32];

} Transaction;

// SEE:https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/primitives/block.h#L68
typedef struct Block
{
	uint32_t	magic;
	uint32_t	blockSize;
	uint16_t	txCount;
	uint8_t		hash[32];
	uint64_t	height;
	BlockHeader header;				// Block header - 80 Bytes
	Transaction *transactions;		// Array of transactions
} Block;

typedef struct Blocks
{
	Block *blocks;
	size_t count;
} Blocks;

// SEE: https://github.com/bitcoin/bitcoin/blob/60b816439eb4bd837778d424628cd3978e0856d9/src/chain.h#L88
// NOTE: This is litterally a copy-paste from bitcoin /src/chain.h - BlockStatus enum
typedef enum BlockStatus : uint32_t
{
	//! Unused.
    BLOCK_VALID_UNKNOWN      =    0,
    //! Reserved (was BLOCK_VALID_HEADER).
    BLOCK_VALID_RESERVED     =    1,
    //! All parent headers found, difficulty matches, timestamp >= median previous, checkpoint. Implies all parents
    //! are also at least TREE.
    BLOCK_VALID_TREE         =    2,
    /**
     * Only first tx is coinbase, 2 <= coinbase input script length <= 100, transactions valid, no duplicate txids,
     * sigops, size, merkle root. Implies all parents are at least TREE but not necessarily TRANSACTIONS.
     *
     * If a block's validity is at least VALID_TRANSACTIONS, CBlockIndex::nTx will be set. If a block and all previous
     * blocks back to the genesis block or an assumeutxo snapshot block are at least VALID_TRANSACTIONS,
     * CBlockIndex::m_chain_tx_count will be set.
     */
    BLOCK_VALID_TRANSACTIONS =    3,
    //! Outputs do not overspend inputs, no double spends, coinbase output ok, no immature coinbase spends, BIP30.
    //! Implies all previous blocks back to the genesis block or an assumeutxo snapshot block are at least VALID_CHAIN.
    BLOCK_VALID_CHAIN        =    4,
    //! Scripts & signatures ok. Implies all previous blocks back to the genesis block or an assumeutxo snapshot block
    //! are at least VALID_SCRIPTS.
    BLOCK_VALID_SCRIPTS      =    5,
    //! All validity bits.
    BLOCK_VALID_MASK         =   BLOCK_VALID_RESERVED | BLOCK_VALID_TREE | BLOCK_VALID_TRANSACTIONS |
                                 BLOCK_VALID_CHAIN | BLOCK_VALID_SCRIPTS,
    BLOCK_HAVE_DATA          =    8, //!< full block available in blk*.dat
    BLOCK_HAVE_UNDO          =   16, //!< undo data available in rev*.dat
    BLOCK_HAVE_MASK          =   BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO,
    BLOCK_FAILED_VALID       =   32, //!< stage after last reached validness failed
    BLOCK_FAILED_CHILD       =   64, //!< descends from failed block
    BLOCK_FAILED_MASK        =   BLOCK_FAILED_VALID | BLOCK_FAILED_CHILD,
    BLOCK_OPT_WITNESS        =   128, //!< block data in blk*.dat was received with a witness-enforcing client
    BLOCK_STATUS_RESERVED    =   256, //!< Unused flag that was previously set on assumeutxo snapshot blocks and their
                                      //!< ancestors before they were validated, and unset when they were validated.
} BlockStatus;

// SEE: https://github.com/bitcoin/bitcoin/blob/f914f1a746d7f91951c1da262a4a749dd3ebfa71/src/chain.h#L95
typedef struct BlockIndexRecord
{
	uint8_t blockHash[32];			// 32 bytes - Block Hash -- Manually reversed it
	BlockHeader blockHeader;		// 80 bytes - Raw Block Header
	uint64_t version;				// 8 bytes - Client version
	uint64_t height;				// 8 bytes - Block Height
	uint64_t txCount;				// 8 bytes - number of transactions in the block
	BlockStatus validationStatus;	// 4 bytes - validation status (enum)
	uint64_t blockFile;				// 8 bytes - block file number
	uint64_t blockOffset;			// 8 bytes - where the block is located in blk.dat
	uint64_t undoFile;				// 8 bytes - undo file number
	uint64_t undoOffset;			// 8 bytes - where the block is located in rev.dat
} BlockIndexRecord;					// 174 bytes

typedef struct FileInformationRecord
{
	uint64_t filenumber;			// 8 bytes - file number
	uint64_t blockCount;			// 8 bytes - The number of blocks stored in the block file with that number
	uint64_t dataFileSize;			// 8 bytes - The size of the blk.dat file with that number
	uint64_t revFileSize;			// 8 bytes - The size of the rev.dat file with that number
	uint64_t lowestHeight;			// 8 bytes - The lowest height of blocks stored in the block file with that number
	uint64_t highestHeight;			// 8 bytes - The highest height of blocks stored in the block file with that number
	uint64_t lowestTimestamp;		// 8 bytes - The lowest timestamp of blocks stored in the block file with that number
	uint64_t highestTimestamp;		// 8 bytes - The highest timestamp of blocks stored in the block file with that number
} FileInformationRecord;			// 64 bytes

typedef struct TxIndex
{
	uint8_t txHash[32];
	uint64_t blockFile;
	uint64_t txOffset;
	uint64_t txIndex;
} TxIndex;

// FIXME: Check for actual ChainstateIndex structure used by BitcoinCore for levelDB values.
typedef struct ChainstateIndex
{
	uint8_t txHash[32];
	uint64_t nVersion;
	uint64_t nCode;
} ChainstateIndex;

typedef struct IndexRecords
{
	BlockIndexRecord *blockIndexRecord;
	uint64_t blockIndexRecordCount;
	FileInformationRecord *fileInformationRecord;
	uint64_t fileInformationRecordCount;
	TxIndex *txIndexRecord;
	uint64_t txIndexRecordCount;
} IndexRecords;


// WARN: All fee related statistics are hard to compute because it's hard to know the input amounts.
typedef struct BlockStats
{
	
	uint64_t avgFee;			// WARN: Probably not easily computable
	uint64_t avgFeeRate;		// WARN: Probably not easily computable
	uint64_t avgTxSize;
	uint8_t blockHash[32];
	uint64_t height;
	uint64_t ins;
	uint64_t maxFee;			// WARN: Probably not easily computable
	uint64_t maxTxSize;
	uint64_t minFee;			// WARN: Probably not easily computable
	uint64_t minFeeRate;		// WARN: Probably not easily computable
	uint64_t outs;
	uint64_t subsidy;
	uint64_t swTotalSize;
	uint64_t swTotalWeight;
	uint64_t swTx;
	uint64_t time;
	uint64_t totalOut;
	uint64_t totalSize;
	uint64_t totalWeight;
	uint64_t totalFee;			// WARN: Probably not easily computable
	uint64_t txs;
	uint64_t utxoIncrease;
} BlockStats;




// INFO: Free allocated structures
void FreeFileList(FileList *fileList);

void FreeInput(Input *input);
void FreeOutput(Output *output);
void FreeWitness(Witness *witness);
void FreeTransaction(Transaction *transaction);
void FreeBlockHeader(BlockHeader *blockHeader);
void FreeBlock(Block *block);

#endif // !STRUCTS_H

