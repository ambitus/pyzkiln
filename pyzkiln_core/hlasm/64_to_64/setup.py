import os
from setuptools import setup, Extension


def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))          # obtain the current directory
    extension_path = os.path.join(dir_path, "hlasm_extension.c")    # add the extension file to the path
    assembly_path = os.path.join(dir_path, "asm.s")                 # add the extension file to the path

    setup(
        name="hlasm_extension",
        version="1.0.0",
        ext_modules=[
            Extension(
                "hlasm_extension",                          # name
                sources = [extension_path, assembly_path],  # path to extension
            )
        ],
    )

if __name__ == "__main__":
    main()
