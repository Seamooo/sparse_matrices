#colours
red=$'\e[0;31m'
green=$'\e[1;32m'
end=$'\e[0m'

TESTS=(
#scalar multiplication tests
"./sparse_matrix.bin --sm 3.5 -f int64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm 3.5 -f int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm 3.5 -f float64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm 3.5 -f float64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm 3 -f int64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm 3 -f int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm 3 -f float64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm 3 -f float64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm -3.5 -f int64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm -3.5 -f int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm -3.5 -f float64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm -3.5 -f float64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm -3 -f int64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm -3 -f int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --sm -3 -f float64.in --format COO -s -t 4"
"./sparse_matrix.bin --sm -3 -f float64.in --format COO -s --nothreading"
#trace tests
"./sparse_matrix.bin --tr -f int64.in --format COO -s -t 4"
"./sparse_matrix.bin --tr -f int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --tr -f float64.in --format COO -s -t 4"
"./sparse_matrix.bin --tr -f float64.in --format COO -s --nothreading"
#transpose tests
"./sparse_matrix.bin --ts -f int64.in --format COO -s -t 4"
"./sparse_matrix.bin --ts -f int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --ts -f float64.in --format COO -s -t 4"
"./sparse_matrix.bin --ts -f float64.in --format COO -s --nothreading"
#matrix addition tests
"./sparse_matrix.bin --ad -f int64.in int64.in --format COO -s -t 4"
"./sparse_matrix.bin --ad -f int64.in int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --ad -f float64.in float64.in --format COO -s -t 4"
"./sparse_matrix.bin --ad -f float64.in float64.in --format COO -s --nothreading"
#matrix multiplication tests
"./sparse_matrix.bin --mm -f int64.in int64.in --format COO -s -t 4"
"./sparse_matrix.bin --mm -f int64.in int64.in --format COO -s --nothreading"
"./sparse_matrix.bin --mm -f float64.in float64.in --format COO -s -t 4"
"./sparse_matrix.bin --mm -f float64.in float64.in --format COO -s --nothreading"
#basic tests -s flag only included not to spam the screen
"./sparse_matrix.bin --sm 3.5 -f int64.in -s"
"./sparse_matrix.bin --sm 3 -f int64.in -s"
"./sparse_matrix.bin --sm 3.5 -f float64.in -s"
"./sparse_matrix.bin --sm 3 -f float64.in -s"
"./sparse_matrix.bin --sm -3.5 -f int64.in -s"
"./sparse_matrix.bin --sm -3 -f int64.in -s"
"./sparse_matrix.bin --sm -3.5 -f float64.in -s"
"./sparse_matrix.bin --sm -3 -f float64.in -s"
"./sparse_matrix.bin --tr -f int64.in -s"
"./sparse_matrix.bin --tr -f float64.in -s"
"./sparse_matrix.bin --ts -f int64.in  -s"
"./sparse_matrix.bin --ts -f float64.in -s"
"./sparse_matrix.bin --ad -f int64.in int64.in -s"
"./sparse_matrix.bin --ad -f float64.in float64.in -s"
"./sparse_matrix.bin --mm -f int64.in int64.in -s"
"./sparse_matrix.bin --mm -f float64.in float64.in -s"
)
for ((i=0; i<${#TESTS[@]}; ++i));
do
	${TESTS[i]};
	case "$?" in
		0)
			echo "${TESTS[i]} ${green}Success${end}";;
		1)
			echo "${TESTS[i]} ${red}Error${end}";;
	esac;
done;
