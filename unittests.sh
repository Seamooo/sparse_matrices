#colours
red=$'\e[0;91m'
green=$'\e[1;32m'
end=$'\e[0m'
SINGLE_FILE_OPERATION=(
	"--sm 3.5"
	"--sm 3"
	"--sm -3.5"
	"--sm -3"
	"--ts"
);
SFO_LEN=${#SINGLE_FILE_OPERATION[@]};
FORMAT=(
	""
	"--format COO"
	"--format CSR"
	"--format CSC"
);
FORMAT_LEN=${#FORMAT[@]};
THREADING=(
	""
	"-t 5"
	"--nothreading"
);
THREADING_LEN=${#THREADING[@]};
SINGLE_FILES=(
	"-f int64.in"
	"-f float64.in"
	"-f int64x32.in"
);
SF_LEN=${#SINGLE_FILES[@]}
TR_FILES=(
	"-f int64.in"
	"-f float64.in"
);
TRF_LEN=${#TR_FILES[@]}
MM_FILES=(
	"-f int32x64.in int64x32.in"
	"-f int64.in int64.in"
	"-f float64.in float64.in"
);
MMF_LEN=${#MM_FILES[@]};
AD_FILES=(
	"-f int64.in int64.in"
	"-f float64.in float64.in"
	"-f int32x64.in int32x64.in"
);
ADF_LEN=${#AD_FILES[@]};
NUM_SFO_TESTS=$(($SFO_LEN*$SF_LEN*$FORMAT_LEN*$THREADING_LEN))
for ((i=0; i<NUM_SFO_TESTS; ++i));
do
	TEMP=$(($i));
	THREADING_i=$(($TEMP % $THREADING_LEN));
	TEMP=$(($TEMP / $THREADING_LEN));
	FORMAT_i=$(($TEMP % $FORMAT_LEN));
	TEMP=$(($TEMP / $FORMAT_LEN));
	SF_i=$(($TEMP % $SF_LEN));
	TEMP=$(($TEMP / $SF_LEN));
	SFO_i=$(($TEMP % $SFO_LEN));
	TEST="./sparse_matrix.bin ${SINGLE_FILE_OPERATION[SFO_i]} ${SINGLE_FILES[SF_i]} ${FORMAT[FORMAT_i]} ${THREADING[THREADING_i]} -s"
	$TEST;
	case "$?" in
		0)
			echo "$TEST ${green}Success${end}";;
		*)
			echo "$TEST ${red}Error${end}";;
	esac;
done;
NUM_TR_TESTS=$(($TRF_LEN*$FORMAT_LEN*THREADING_LEN))
for((i = 0; i < NUM_TR_TESTS; ++i))
do
	TEMP=$(($i));
	THREADING_i=$(($TEMP % $THREADING_LEN));
	TEMP=$(($TEMP / $THREADING_LEN));
	FORMAT_i=$(($TEMP % $FORMAT_LEN));
	TEMP=$(($TEMP / $FORMAT_LEN));
	TRF_i=$((TEMP % TRF_LEN));
	TEST="./sparse_matrix.bin --tr ${TR_FILES[TRF_i]} ${FORMAT[FORMAT_i]} ${THREADING[THREADING_i]} -s"
	$TEST;
	case "$?" in
		0)
			echo "$TEST ${green}Success${end}";;
		*)
			echo "$TEST ${red}Error${end}";;
	esac;
done;
NUM_AD_TESTS=$(($ADF_LEN*$FORMAT_LEN*$THREADING_LEN))
for ((i = 0; i<NUM_AD_TESTS; ++i));
do
	TEMP=$(($i));
	THREADING_i=$(($TEMP % $THREADING_LEN));
	TEMP=$(($TEMP / $THREADING_LEN));
	FORMAT_i=$(($TEMP % $FORMAT_LEN));
	TEMP=$(($TEMP / $FORMAT_LEN));
	ADF_i=$((TEMP % ADF_LEN));
	TEST="./sparse_matrix.bin --ad ${AD_FILES[ADF_i]} ${FORMAT[FORMAT_i]} ${THREADING[THREADING_i]} -s"
	$TEST;
	case "$?" in
		0)
			echo "$TEST ${green}Success${end}";;
		*)
			echo "$TEST ${red}Error${end}";;
	esac;
done;
NUM_MM_TESTS=$(($MMF_LEN*$FORMAT_LEN*$THREADING_LEN))
for ((i = 0; i<NUM_AD_TESTS; ++i));
do
	TEMP=$(($i));
	THREADING_i=$(($TEMP % $THREADING_LEN));
	TEMP=$(($TEMP / $THREADING_LEN));
	FORMAT_i=$(($TEMP % $FORMAT_LEN));
	TEMP=$(($TEMP / $FORMAT_LEN));
	MMF_i=$((TEMP % MMF_LEN));
	TEST="./sparse_matrix.bin --mm ${MM_FILES[MMF_i]} ${FORMAT[FORMAT_i]} ${THREADING[THREADING_i]} -s"
	$TEST;
	case "$?" in
		0)
			echo "$TEST ${green}Success${end}";;
		*)
			echo "$TEST ${red}Error${end}";;
	esac;
done;
