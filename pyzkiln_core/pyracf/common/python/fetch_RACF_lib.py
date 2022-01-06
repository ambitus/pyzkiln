"""
  fetch_RACF_lib.py - A helper function to fetch the C library that makes the call to 
  RACF.

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""
import ctypes
import pathlib
import sys 

def fetch_RACF_lib():
    libname = pathlib.Path().absolute() / "libracf.so"
    print("--- RACF library: %s" % (libname))
    ctypes.cdll.LoadLibrary(libname)
    libracf = ctypes.CDLL(libname)
    print("After libracf.so fetch")
    return libracf