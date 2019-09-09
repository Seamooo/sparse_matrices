Sparse Matrices
===============

Supported on macintosh, linux, windows

mac/linux compilation:  
`make`

Windows compilation:  
open win.cfg and change vs_path to the directory containing visual studio  
startmake.bat

Usage
-----
`./sparse_matrix.bin <--tr | --ad | ...> -f [file1] [file2]`

Additional Flags
----------------
`-t` specify number of threads  
`-l` log output  
`--format <COO | CSR | ...> [BCSR BLOCK SIZE]` specify matrix format  
`--nothreading` run with no threading  
`-s` run with silenced output  


Supported sparse matrix formats
-------------------------------
Coordinate Format (COO)  
Compressed Sparse Row (CSR) --not implemented  
Compressed Sparse Columb (CSC) --not implemented  
Block Compressed Row Storage (BCRS) --not implemented  
Compressed Diagonal Storage (CDS) --not implemented  
Jagged Diagonal Storage (JDS) --not implemented  
Skyline Storage (SKS) --not implemented  
  
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
  
Resources:
----------
[sciencedirect.com/topics/computer-science/sparse-matrix-computation](sciencedirect.com/topics/computer-science/sparse-matrix-computation)
