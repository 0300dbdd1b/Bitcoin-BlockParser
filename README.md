## Installation
 ### Install LevelDB
    '''
    https://github.com/google/leveldb
    '''


## TODOS:

[x] Read blocks/index/*ldb files
[x] Index BlockIndexRecord
[ ] Index FileInformationRecord
[ ] Autodetect Bitcoin datadir
[ ] Design parser for querying
[ ] Read blk*.dat files
[ ] Handle multiple formats

Parser:
    [A      : Including A
    A[      : Excluding A
    A-B     : From A to B
    A:      : From A
    A:*     : All from A
    SUM(A)  : Sum A
    COUNT(A): Count A

ex:
    SUM([0-100]:TX:outputs:value)
-- Sum all the outputs value of all the transactions in blocks 0 to 100 included
    [100-200]:*]TX:outputs:scriptPubkey [200-300]:COUNT(TX)
-- All the data of blocks 100 to 200 without the outputs scriptPubkey then the count of transactions of blocks 200 to 300
