#Copyright IBM Corp. 2024.

import ctypes                          # ctypes: library used in Python to load functions from shared libraries
mydll = ctypes.CDLL("./cobtest.so")    # CDLL loads the shared library into the variable mydll (cobtest.so - this file is created from compiling cobtest.cbl)

# Set up the function argument and return types
mydll.COBTEST.argtypes = [ctypes.c_int, ctypes.c_int]
mydll.COBTEST.restype = ctypes.c_int

f = mydll.COBTEST(5,6)
print(f)                               # The expected output is the addition of the two paramters - hence in this case it will be 11
