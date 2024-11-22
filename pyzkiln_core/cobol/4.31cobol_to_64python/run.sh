#Copyright IBM Corp. 2024.

# Clean up directory
rm -f *.o *.x CEE* *.lst cobtest cobtest2 SHAREDL

set -x
set -e

# Compile COBOL 31bit program
cob2 -g      -comprc_ok=0 -q"list"                                      cobtest.cbl -o cobtest

# Compile COBOL 64bit program
cob2 -g -q64 -comprc_ok=0 -q"list"                                -bdll cobtest2.cbl -o cobtest2

# Compile COBOL 64bit shared lib
cob2 -g -q64 -comprc_ok=0 -q"list" -q"dll" -q"pgmname(longmixed)" -bdll -qexportall sharedlib.cbl -o SHAREDL ${PYTHON_PATH}/lib/libpython3.12.x

# Create a data set
YOUR_DS=$USER.DATA.FILE
tso "alloc DSN('${YOUR_DS}') NEW SPACE(10,10) DIR(10) UNIT(SYSDA) RECFM(U) BLKSIZE(4096) DSNTYPE(LIBRARY)"
cp cobtest2 "//'$YOUR_DS(COBTEST2)'"
cp SHAREDL  "//'$YOUR_DS(SHAREDL)'"

# Run
export STEPLIB=${YOUR_DS}:$STEPLIB
export LIBPATH=$LIBPATH:.
export _IGZ_RUNOPTS="AMODE3164"
./cobtest
