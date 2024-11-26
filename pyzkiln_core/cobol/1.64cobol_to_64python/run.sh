#Copyright IBM Corp. 2024.

rm -f *.o *.x *.lst cobtest

set -x
set -e

# Compile COBOL which embeds Python
cob2 -q64 -comprc_ok=0 -q"list" -q"dll" -q"pgmname(longmixed)" cobtest.cbl -o cobtest ${PYTHON_PATH}/lib/libpython3.12.x

# Run 
# Expected outout is a simple message 'Hello world' printed out in the terminal
./cobtest
