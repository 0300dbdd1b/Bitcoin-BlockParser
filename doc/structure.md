##


|   Operating System    | Data Directory                                |
|-----------------------| ----------------------------------------------|
|       Windows         | `%LOCALAPPDATA%\Bitcoin\`                     |
|       Linux           | `$HOME/.bitcoin/`                             |
|       MacOs           | `$HOME/Library/Application Support/Bitcoin/`  |

```
├── bitcoin.conf
├── blocks
│   ├── blk*.dat
│   ├── rev*.dat
│   └── index
│       └── *.ldb
├── chainstate
│   └── *.ldb
├── indexes
│   ├── coinstats
│   │   └─── *.ldb
│   └── txindex
│       └─── *.ldb
├── db.log
├── debug.log
├── fee_estimates.dat
├── mempool.dat
├── peers.dat
├── banlist.dat
└── wallet.dat
```




