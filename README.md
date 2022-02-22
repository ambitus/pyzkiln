# pyzkiln_core

A place to create bricks that will build new structures on z/OS.

People want to manage resources on z/OS like they manage resources on any other Cloud platform. To do this effectively, we need to create capabilities that hide things like JCL and assembler macros from management tools. This effort is dedicated to creating be basic bricks that can be used to build those new capabilities on the platform.

Imagine managing resources without every writing JCL? Imagine automating management tasks using the same scripting tools that are common on other platforms. Imagine using automation tools like Ansible to manage z/OS resources. This repository will contain the collective Python components from multiple businesses. Together we can build a new management infrastructure for z/OS.

Consider contributing to this repository. Anyone interested in creating Python building blocks for managing z/OS resources is welcome to contribute.

## Pre-requisites

The tools in this repository will rely on:

- [Open Automation Utilities (ZOAU)](https://www.ibm.com/docs/en/zoau/1.2.0)
  - Must enable ZOAU Python APIs (zoautil_py) by installing and configuring pip installable package. For more information visit [Installing and configuring ZOA Utilities](https://www.ibm.com/support/knowledgecenter/SSKFYE_1.1.0/install.html)
- [IBM Open Entprise SDK for Python](https://www.ibm.com/products/open-enterprise-python-zos)

## Pyzkiln Content

Pyzkiln-core package is made up of the following:
  - zcmds - Modules that return the output from a set of z/os operator commands in an array of Python dictionaries
  - zutils - Modules with the purpose to serve as Z automation building blocks
  - pyracf - Python interface to a set of RACF functionality (work in progress)

More detailed documentation under [/docs](https://github.com/ambitus/pyzkiln/blob/main/docs/index.md)  


## Installation:

1. Build Manually using setup.py 

Download package ZIP file or clone. In z/OS system, navigate to root directory of pyzkiln package and issue the following command:

    python3 setup.py sdist bdist_wheel

Navigate into /dist directory and issue the following command 

    pip install pyzkiln_core-0.1-py3-none-any.whl

2. Use provided Wheel 

Download the wheel located in /dist. In z/OS system, navigate to the path where wheel is located and issue the following command:

    pip install pyzkiln_core-0.1-py3-none-any.whl
  
## Pyracf configuration 

pyracf will need the following environment variables configured:
```
PY_RACF_HOME=<location of pyracf root>
PYRACF_HOME=<location of pyracf root>
PYTHONPATH=$PYTHONPATH:$PY_RACF_HOME:PY_RACF_HOME/common/python:$PY_RACF_HOME/R_admin/python
LIBPATH=$LIBPATH:$PYRACF_HOME
```

example .profile 
```
PY_RACF_HOME=/u/luisam/working_env/lib/python3.10/site-packages/pyzkiln_core/pyracf
export PY_RACF_HOME
export PYRACF_HOME=${PY_RACF_HOME}
export PYTHONPATH=${PY_RACF_HOME}:${PY_RACF_HOME}/common/python:${PY_RACF_HOME}/R_admin/python:$PYTHONPATH
export LIBPATH=${PY_RACF_HOME}:$LIBPATH
```

Note: to find location of pyracf root use the pip show command 

      pip show pyzkiln-core 

More pyracf documentation can be found [/pyracf](https://github.com/ambitus/pyzkiln/tree/main/pyzkiln_core/pyracf) 

## Usage: example

```python
from pyzkiln_core import display_active_jobs

active_jobs = display_active_jobs()
print(active_jobs[3]['JobName'])
```
