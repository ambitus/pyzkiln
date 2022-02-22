#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess

from setuptools import find_packages, setup
from setuptools.command.build_py import build_py


class PyracfInstallError(Exception):
    pass

class Build(build_py):
    """Customized setuptools install command - builds pyracf code on install."""
    def run(self):
        # temporarily set environment variable PYRACF_HOME
        pyracf_dir = f"{os.path.dirname(__file__)}/pyzkiln_core/pyracf"
        os.environ['PYRACF_HOME'] = pyracf_dir
        cmd = f"make -c {os.path.dirname(__file__)}/pyzkiln_core/pyracf"
        try:
            subprocess.check_output(cmd, shell=True)
        except subprocess.CalledProcessError as e:
            raise PyracfInstallError(f"Error while installing pyracf dependencies, returncode: {e.returncode}")
        build_py.run(self)

setup(
    name = "pyzkiln_core",
    version = "0.1",
    author = "Luisa Martinez, Frank De Gilio",
    author_email = "martinel@us.ibm.com, degilio@us.ibm.com",
    description = ("A set of basic bricks "
                   "to build new capabilities for Z" ),
    license = "Apache", 
    url = "https://github.com/ambitus/pyzkiln", 
    packages = find_packages(),
    long_description = open("README.md", "r").read(),
    long_description_content_type = 'text/markdown',
    include_package_data = True,
    classifiers = [
        "Programming Language :: Python :: 3",
        "License ::  Apache License",
        "Operating System :: z/OS ",
    ],
    python_requires = '>=3.6',
    cmdclass = {
        'build_py': Build,
    }
)

