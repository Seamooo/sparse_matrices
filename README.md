Sparse Matrices
===============

Supported on macintosh, linux, windows

mac/linux compilation:  
`make`

Usage
-----
`./sparse_matrix.[bin | exe] <--tr | --ad | ...> -f <file1> [file2]`

Additional Flags
----------------
`-t %d` specify number of threads  
`-l` log output  
`--format <COO | CSR | ...> [BCSR BLOCK SIZE]` specify matrix format  
`--nothreading` run with no threading  
`-p` run with silenced output  


Supported sparse matrix formats
-------------------------------
Coordinate Format (COO)  
Compressed Sparse Row (CSR) 
Compressed Sparse Column (CSC)  
Compressed Diagonal Storage (CDS) --not implemented  

Known Bugs:
  
Resources:
----------
[https://sciencedirect.com/topics/computer-science/sparse-matrix-computation](https://sciencedirect.com/topics/computer-science/sparse-matrix-computation)  
[https://inf.ethz.ch/personal/markusp/teaching/263-2300-ETH-spring11/slides/class15.pdf](https://inf.ethz.ch/personal/markusp/teaching/263-2300-ETH-spring11/slides/class15.pdf)  
[https://peerj.com/articles/cs-151.pdf](https://peerj.com/articles/cs-151.pdf)  
[https://www.geeksforgeeks.org/operations-sparse-matrices/](https://www.geeksforgeeks.org/operations-sparse-matrices/)

