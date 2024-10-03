### `blocks/index/*.ldb`

These are files created by levelDB to index blocks.

The key for each block index record begins with the letter 'b' to distinguish it from other types of entries
in the database (e.g., transaction index entries might start with 't').
Following the 'b' is the 32-byte (256-bit) hash of the block. This hash serves as a unique identifier for the block.
The block hash is typically represented in little-endian byte order in the database.

    'b' + 32-byte block hash -> block index record. 
    Each record stores:
        * The client version (VarInt128)
        * The height        (VarInt128)
        * The number of transactions. (VarInt128)
        * To what extent this block is validated.   (VarInt128)
        * In which file, and where in that file, the block data is stored.  (VarInt128) (VarInt128)
        * In which file, and where in that file, the undo data is stored.   (VarInt128) (VarInt128)
        * The block header  (80 bytes)

    'f' + 4-byte file number -> file information record. 
    Each record stores:
        * The number of blocks stored in the block file with that number.
        * The size of the block file with that number ($DATADIR/blocks/blkNNNNN.dat).
        * The size of the undo file with that number ($DATADIR/blocks/revNNNNN.dat).
        * The lowest and highest height of blocks stored in the block file with that number.
        * The lowest and highest timestamp of blocks stored in the block file with that number.

    'l' -> 4-byte file number: the last block file number used.

    'R' -> 1-byte boolean ('1' if true): whether we're in the process of reindexing.

    'F' + 1-byte flag name length + flag name string -> 1 byte boolean ('1' if true, '0' if false):
    various flags that can be on or off.
    Currently defined flags include:
        * 'txindex': Whether the transaction index is enabled.

    't' + 32-byte transaction hash -> transaction index record.
    These are optional and only exist if 'txindex' is enabled (see above).
    Each record stores:
        * Which block file number the transaction is stored in.
        * Which offset into that file the block the transaction is part of is stored at.
        * The offset from the start of that block to the position where that transaction itself is stored.



#### client version

#### block height
A varint indicating how many blocks precede this block in the chain.

#### tx count
A varint indicating the total number of transactions included in the block.

#### validation status
A varint representing flags that indicate:
    - Whether the block's data is fully available (BLOCK_HAVE_DATA)
    - Whether the block's undo data (for transaction reversal) is available (BLOCK_HAVE_UNDO)

#### File Location and Position

If the block's data is available:
    - A varint indicating the file number (e.g., blkXXXXX.dat) where the block data is stored.
    - A varint indicating the byte offset within the file where the block data starts.
If the block's undo data is available:
    - A varint indicating the file number (e.g., revXXXXX.dat) where the undo data is stored.
    - A varint indicating the byte offset within the file where the undo data starts.


#### block header
80 bytes in total Contains the following fields:
    -   version                 -   4 bytes
    -   previous block hash     -   32 bytes
    -   merkle root             -   32 bytes
    -   timestamp               -   4 bytes
    -   bits                    -   4 bytes
    -   nonce                   -   4 bytes

>   Important Notes:
>
>   Varints:
>       Bitcoin uses varints for space efficiency. 
>       Smaller numbers are encoded with fewer bytes than larger ones. 
>       This makes the size of the block index record variable depending on the block height, tx count, etc.
>
>   Data Availability:
>       Not all block data and undo data might be available in the LevelDB index. 
>       The validation status flags indicate whether the data is present and where 
>       to find it in the actual block files on disk.
>
>   Endianness:
>       The block hash and other fields within the block header are stored in little-endian byte order in LevelDB. 
>       This means the least significant byte comes first.


|key                                            |                                       |
|-----------------------------------------------|---------------------------------------|
|   + ???           - ??? bytes ('t')           |   + block file number - ??? bytes     |
|   + tx hash       - 32 bytes                  |   + offset            - ??? bytes     |
|                                               |   + offset from start - ??? bytes     |

### `blocks/chainstate/*.ldb`
These are files created by LevelDB to store the UTXO set.

    'C' + 32-byte transaction hash -> unspent transaction output record for that transaction.
    These records are only present for transactions that have at least one unspent output left.
    Each record stores:
        * The version of the transaction.
        * Whether the transaction was a coinbase or not.
        * Which height block contains the transaction.
        * Which outputs of that transaction are unspent.
        * The scriptPubKey and amount for those unspent outputs.

    'B' -> 32-byte block hash: the block hash up to which the database represents the unspent transaction outputs.

Structure of chainstate levelDB files:

|key                                                | value                                         |
|---------------------------------------------------|-----------------------------------------------|
|(v0.15+):                                          |   ???             - ??? bytes ('B') <- ?      |
|    + ???                  - ??? bytes ('C') <- ?  |   block hash      - 32 bytes                  |
|    + transaction hash     - 32 bytes              |                                               |
|    + output index len     - ??? bytes             |                                               |
|    + output index         - ??? bytes             |                                               |
|    + is coinbase          - ??? bytes             |                                               |
|    + block height         - ??? bytes             |                                               |
|    + scriptPubkey         - ??? bytes             |                                               |
|    + amount               - ??? bytes             |                                               |
|(v0.14-):                                          |                                               |
|    + ???                  - ??? bytes ('c') <- ?  |                                               |
|    + transaction hash     - 32 bytes              |                                               |
|    + version              - ??? bytes             |                                               |
|    + is coinbase          - ??? bytes             |                                               |
|    + block height         - ??? bytes             |                                               |
|    + which uspend outs    - ??? bytes             |                                               |
|    + scriptPubkeys        - ??? bytes             |                                               |
|    + amounts              - ??? bytes             |                                               |

### Use of levelDB
LevelDB is a key-value store that was introduced to store the block index and UTXO set (chainstate) in 2012 as part of the complex "Ultraprune" pull [(PR 1677)](https://github.com/bitcoin/bitcoin/pull/1677/commits).

On the subject of why LevelDB is used, core developer Greg Maxwell stated the following to the [bitcoin-dev mailing list in October 2015](https://bitcoin-development.narkive.com/XAPoxKZU/patch-switching-bitcoin-core-to-sqlite-db):
> I think people are falling into a trap of thinking "It's a <database>, I know a <black box> for that!"; 
> but the application and needs are very specialized here. . . 
> It just so happens that on the back of the very bitcoin specific cryptographic consensus algorithim
> there was a slot where a pre-existing high performance key-value store fit; 
> and so we're using one and saving ourselves some effort...
> One might ask whether different nodes could use different databases - as long as they retrieve the same data, what's the difference? 
> The issue here is "bug-for-bug compatibility" - if one database has a bug that causes records to not be returned under certain circumstances, then all other nodes bst have the same bug, else the network could fork as a result.

Greg Maxwell stated the following in the same thread referenced above [(in response to a proposal to switch to using sqlite)](https://bitcoin-development.narkive.com/XAPoxKZU/patch-switching-bitcoin-core-to-sqlite-db):
> ...Databases sometimes have errors which cause them to fail to return records, or to return stale data. 
> And if those exist consistency must be maintained;
> and "fixing" the bug can cause a divergence in consensus state that could open users up to theft.
> Case in point, prior to leveldb's use in Bitcoin Core it had a bug that, under rare conditions,
> could cause it to consistently return not found on records that were really there...
> Leveldb fixed this serious bug in a minor update.
> But deploying a fix like this in an uncontrolled manner in the bitcoin network would potentially cause a fork in the consensus state; so any such fix would need to be rolled out in an orderly manner.
