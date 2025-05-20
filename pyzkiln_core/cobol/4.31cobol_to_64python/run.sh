# Copyright IBM Corp. 2024.
# Clean up directory
rm -f *.o *.x CEE* *.lst cobtest dycallee

set -x

# Compile COBOL 31bit program
cob2 cobtest.cbl -o cobtest

# Compile COBOL 64bit shared lib
cob2 -q64 -q"pgmname(longmixed)" -bdll dycallee.cbl -o dycallee ${PYTHON_PATH}/lib/libpython3.12.x

# Create a data set
YOUR_DS=$USER.DATA.FILE
tso "alloc DSN('${YOUR_DS}') NEW SPACE(10,10) DIR(10) UNIT(SYSDA) RECFM(U) BLKSIZE(4096) DSNTYPE(LIBRARY)"
cp dycallee "//'$YOUR_DS(DYCALLEE)'"

# Run
export STEPLIB=${YOUR_DS}:$STEPLIB
export _IGZ_RUNOPTS="AMODE3164"
./cobtest
