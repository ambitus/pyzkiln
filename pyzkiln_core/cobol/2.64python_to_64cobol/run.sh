#Copyright IBM Corp. 2024.

# Clean up previous builds
rm -f *.o *.so *.x *.lst

set -x
set -e

# Build COBOL shared library
cob2 -q64 -comprc_ok=0 -q"list" -q"dll" -q"pgmname(longmixed)" -bdll -qexportall cobtest.cbl -o cobtest.so

# Run Python to call the 64bit COBOL
python3 call_cobtest.py
