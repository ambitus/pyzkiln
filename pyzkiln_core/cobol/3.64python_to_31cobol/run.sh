#Copyright IBM Corp. 2024.

rm -f *.o *.x *.so *.lst
rm -rf venv callcobol/callcobol.egg-info callcobol/build callcobol/callcobol.cpython-312.x

set -x
set -e

python3 -m venv venv
source ./venv/bin/activate

# Build our COBOL module
cob2 -comprc_ok=0 -q"list" -q"dll" -q"pgmname(longmixed)" -bdll -qexportall cobtest.cbl -o cobtest.so

# Build our Python package
pip3 install -v ./callcobol

# Call the 31bit COBOL from Python
python3 -c "import callcobol; print(callcobol.call_cobtest(5,6))"
