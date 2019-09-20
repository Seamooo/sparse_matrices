#scalar multiply tests
echo ./sparse_matrix.bin --sm 3.5 -f int64.in --format COO -s -t 4
./sparse_matrix.bin --sm 3.5 -f int64.in --format COO -s -t 4
echo ./sparse_matrix.bin --sm 3.5 -f int64.in --format COO -s --nothreading
./sparse_matrix.bin --sm 3.5 -f int64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --sm 3.5 -f float64.in --format COO -s -t 4
./sparse_matrix.bin --sm 3.5 -f float64.in --format COO -s -t 4
echo ./sparse_matrix.bin --sm 3.5 -f float64.in --format COO -s --nothreading
./sparse_matrix.bin --sm 3.5 -f float64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --sm 3 -f int64.in --format COO -s -t 4
./sparse_matrix.bin --sm 3 -f int64.in --format COO -s -t 4
echo ./sparse_matrix.bin --sm 3 -f int64.in --format COO -s --nothreading
./sparse_matrix.bin --sm 3 -f int64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --sm 3 -f float64.in --format COO -s -t 4
./sparse_matrix.bin --sm 3 -f float64.in --format COO -s -t 4
echo ./sparse_matrix.bin --sm 3 -f float64.in --format COO -s --nothreading
./sparse_matrix.bin --sm 3 -f float64.in --format COO -s --nothreading

#trace tests
echo ./sparse_matrix.bin --tr -f int64.in --format COO -s -t 4
./sparse_matrix.bin --tr -f int64.in --format COO -s -t 4
echo ./sparse_matrix.bin --tr -f int64.in --format COO -s --nothreading
./sparse_matrix.bin --tr -f int64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --tr -f float64.in --format COO -s -t 4
./sparse_matrix.bin --tr -f float64.in --format COO -s -t 4
echo ./sparse_matrix.bin --tr -f float64.in --format COO -s --nothreading
./sparse_matrix.bin --tr -f float64.in --format COO -s --nothreading

#transpose tests
echo ./sparse_matrix.bin --ts -f int64.in --format COO -s -t 4
./sparse_matrix.bin --ts -f int64.in --format COO -s -t 4
echo ./sparse_matrix.bin --ts -f int64.in --format COO -s --nothreading
./sparse_matrix.bin --ts -f int64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --ts -f float64.in --format COO -s -t 4
./sparse_matrix.bin --ts -f float64.in --format COO -s -t 4
echo ./sparse_matrix.bin --ts -f float64.in --format COO -s --nothreading
./sparse_matrix.bin --ts -f float64.in --format COO -s --nothreading

#matrix addition tests
echo ./sparse_matrix.bin --ad -f int64.in int64.in --format COO -s -t 4
./sparse_matrix.bin --ad -f int64.in int64.in --format COO -s -t 4
echo ./sparse_matrix.bin --ad -f int64.in int64.in --format COO -s --nothreading
./sparse_matrix.bin --ad -f int64.in int64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --ad -f float64.in float64.in --format COO -s -t 4
./sparse_matrix.bin --ad -f float64.in float64.in --format COO -s -t 4
echo ./sparse_matrix.bin --ad -f float64.in float64.in --format COO -s --nothreading
./sparse_matrix.bin --ad -f float64.in float64.in --format COO -s --nothreading

#matrix multiply tests
echo ./sparse_matrix.bin --mm -f int64.in int64.in --format COO -s -t 4
./sparse_matrix.bin --mm -f int64.in int64.in --format COO -s -t 4
echo ./sparse_matrix.bin --mm -f int64.in int64.in --format COO -s --nothreading
./sparse_matrix.bin --mm -f int64.in int64.in --format COO -s --nothreading
echo ./sparse_matrix.bin --mm -f float64.in float64.in --format COO -s -t 4
./sparse_matrix.bin --mm -f float64.in float64.in --format COO -s -t 4
echo ./sparse_matrix.bin --mm -f float64.in float64.in --format COO -s --nothreading
./sparse_matrix.bin --mm -f float64.in float64.in --format COO -s --nothreading

