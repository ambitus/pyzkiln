
from setuptools import setup, find_packages

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name = "pyzkiln_core",
    version = "0.1",
    author = "Luisa Martinez, Frank De Gilio, Lei Wang",
    author_email = "martinel@us.ibm.com, degilio@us.ibm.com, wlwangwl@cn.ibm.com",
    description = ("A set of basic bricks "
                   "to build new capabilities for Z" ),
    license = "Apache", 
    Keywords = "zos automation",
    url = "https://github.com/ambitus/pyzkiln", 
    packages = find_packages(),
    long_description = long_description,
    long_description_content_type = 'text/markdown',
    classifiers = [
        "Programming Language :: Python :: 3",
        "License ::  Apache License",
        "Operating System :: z/OS ",
    ],
#    install_requires =[], 
#    entry_points = {  'console_scripts':[],},
    python_requires = '>=3.6',

)

