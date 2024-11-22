#Copyright IBM Corp. 2024.

import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext 
from setuptools import setup 

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    extension_path = os.path.join(dir_path, "src", "callcobol.c")

    setup(
        name="callcobol",
        version="1.0.0",
        ext_modules=[
            Extension(
                "callcobol",            # name
                sources=[extension_path],   # path to extension
            )
        ],
    )

if __name__ == "__main__":
    main()
