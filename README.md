# pyzkiln_core

A place to create bricks that will build new structures on z/OS.

People want to manage resources on z/OS like they manage resources on any other Cloud platform. To do this effectively, we need to create capabilities that hide things like JCL and assembler macros from management tools. This effort is dedicated to creating be basic bricks that can be used to build those new capabilities on the platform.

Imagine managing resources without every writing JCL? Imagine automating management tasks using the same scripting tools that are common on other platforms. Imagine using automation tools like Ansible to manage z/OS resources. This repository will contain the collective python components from multiple businesses. Together we can build a new management infrastructure for z/OS.

Consider contributing to this repository. Anyone interested in creating python building blocks for managing z/OS resources is welcome to contribute.

## Pre-requisites:

The tools in this repository will rely on:

- [Open Automation Utilities (ZOAU)](https://www.ibm.com/support/knowledgecenter/en/SSKFYE_1.1.0/welcome_zoautil.html)
  - Must enable ZOAU Python APIs (zoautil_py) by installing and configuring pip installable package. For more information visit [Installing and configuring ZOA Utilities](https://www.ibm.com/support/knowledgecenter/SSKFYE_1.1.0/install.html)
- [IBM Open Entprise SDK for Python](https://www.ibm.com/products/open-enterprise-python-zos)

## Installation:

For pyzkiln_core package:

    python setup.py install
  
To install requirements: 

    pip install -r requirements.txt



## Usage: example
```python
from pyzkiln_core import displayActiveJobs

activeJobs = displayActiveJobs()
print(activeJobs[3]['JobName'])
```
