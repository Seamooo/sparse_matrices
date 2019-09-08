Sparse Matrices
===============

Supported on macintosh, linux, windows

mac/linux compilation
`make all`

windows compilation
open win.cfg and change vs_path to the directory containing visual studio
open make.bat

supported sparse matrix formats:
Coordinate Format (COO)
Compressed Sparse Row (CSR)
Compressed Sparse Columb (CSC)
Block Compressed Row Storage (BCRS)
Compressed Diagonal Storage (CDS)
Jagged Diagonal Storage (JDS)
Skyline Storage (SKS)

Additional Flags
----------------
specify format:
`--format <[COO,CSR,... etc]>`
run with no threading
`--nothreading`
Run with silenced output
`-s`

if time permits this is the full list of formats I would want to implement:
BCCS Block Compressed Column Storage format
BCRS Block Compressed Row Storage format
BND  Linpack Banded format
BSR  Block Sparse Row format
CCS  Compressed Column Storage format
COO  Coordinate format
CRS  Compressed Row Storage format
CSC  Compressed Sparse Column format
CSR  Compressed Sparse Row format
DIA  Diagonal format
DNS  Dense format
ELL  Ellpack-Itpack generalized diagonal format
JAD  Jagged Diagonal format
LNK  Linked list storage format
MSR  Modified Compressed Sparse Row format
NSK  Nonsymmetric Skyline format
SSK  Symmetric Skyline format
SSS  Symmetric Sparse Skyline format
USS  Unsymmetric Sparse Skyline format
VBR  Variable Block Row format
