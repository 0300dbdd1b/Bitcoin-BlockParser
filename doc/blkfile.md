# `blocks/blk*.dat`

The blk.dat files in the ~/.bitcoin/blocks/ directory contain the raw block data received by your Bitcoin Core node.
These blk.dat files basically store the entire blockchain.

## location
Where is the blockchain stored on your computer?

The location of the raw blockchain files on your disk depends on what operating system you're using. These are the default locations:

 OS      | Location                                          |
---------|---------------------------------------------------|
 Linux   | ~/.bitcoin/blocks/                                |
 Mac     | ~/Library/Application Support/Bitcoin/blocks/     |
 Windows | %APPDATA%\Bitcoin\blocks\                         |

You can change the location of the block data directory by setting the `datadir=<dir>` option in the [bitcoin.conf configuration file](bitcoin-conf.md).

## Filenames
How are the blockchain files organized?

Every block that your node receives gets appended to a blk.dat file. But instead of the entire blockchain being stored in one massive file, they are split in to multiple blk*.dat files.
```
~/.bitcoin/blocks/
blk00000.dat
blk00001.dat
blk00002.dat
blk00003.dat
blk00004.dat
and so on...
```
Your node first adds blocks to blk00000.dat, then when it fills up it moves on to blk00001.dat, then blk00002.dat..., and so on. If you're on Linux, you can navigate to the data directory and list all the raw block files with:
```
$ cd ~/.bitcoin/blocks/
$ ls blk*

blk00000.dat
blk00001.dat
blk00002.dat
blk00003.dat
blk00004.dat
blk00005.dat
blk00006.dat
...
```
The maximum blk.dat file size is 128 MiB (134,217,728 bytes). This limit is set by [MAX_BLOCKFILE_SIZE](.

## Example
What does a raw block look like?

The data in blk.dat files is stored in binary, which is basically a bunch of 1s and 0s and not human-readable text.

Nonetheless, we can look at the genesis block by reading the first 293 bytes of blk00000.dat. 
I've split up the individual fields so you can see them more clearly:
```
f9beb4d9 1d010000 01000000 0000000000000000000000000000000000000000000000000000000000000000 3ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a 29ab5f49 ffff001d 1dac2b7c 01 01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff4d04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000
```

## Structure
What is the structure of a raw block?

Diagram showing structure of the raw block data inside the blk.dat files.
The data above can be split in to five parts:

- The magic bytes (4 bytes) is a message delimiter indicating the start of a block.
- The size (4 bytes) indicates the size of the upcoming block in bytes.
- The block header (80 bytes) is the summary of the block data.
- The tx count (compact size) indicates how many transactions are in the block.
- The transaction data (variable) is all of the transactions in the block concatenated one after the other.

The size field is what allowed me to figure out that I needed to read 293 bytes to get the whole block in the example above. The size of the block is indicated as 1d010000, so to get this in human format:

Convert 1d010000 from little-endian to big-endian to get 0000011d
Convert 0000011d from hexadecimal to decimal to get 285
So the actual block itself is only 285 bytes. But then there is an extra 8 bytes at the start for the magic-bytes + size, so I needed to read 293 bytes from the start of the raw blockchain file to get the full block of data.

