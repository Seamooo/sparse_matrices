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
`./sparse_matrix.[bin | exe] <--tr | --ad | ...> -f <file1> [file2]`

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
Compressed Sparse Row (CSR) 
Compressed Sparse Column (CSC)  
Compressed Diagonal Storage (CDS) --not implemented  

Known Bugs:
-----------
scalar multiple not included does not throw error  
  
Resources:
----------
[https://sciencedirect.com/topics/computer-science/sparse-matrix-computation](https://sciencedirect.com/topics/computer-science/sparse-matrix-computation)  
[https://inf.ethz.ch/personal/markusp/teaching/263-2300-ETH-spring11/slides/class15.pdf](https://inf.ethz.ch/personal/markusp/teaching/263-2300-ETH-spring11/slides/class15.pdf)  
[https://peerj.com/articles/cs-151.pdf](https://peerj.com/articles/cs-151.pdf)  
[https://www.geeksforgeeks.org/operations-sparse-matrices/](https://www.geeksforgeeks.org/operations-sparse-matrices/)

