# Contributing to pyzkiln 

Thank you for taking the time to contribute to pyzkiln!
The following are a set of guidelines to help you contribute.

**Table Of Contents**

* [Before getting started](#before-getting-started)

* [Ways to contribute](#ways-to-contribute)

* [Style Guidelines](#style-guidelines)

* [Contribution checklist](#contribution-checklist)

* [Found a bug?](#found-a-bug)


## Before getting started
pyzkiln is a place to create Python building blocks that will be used to build new structures on z/OS. 

The pyzkiln package consist of two set of functionalities **zcmds** and **zutils**. 

### **zcmds**

A set of z/OS operator commands that return an array of Python dictionaries. Dictionaries allow commands to be easily included in automation for easy parsing and data manipulation. 

### **zutils**
A set of Python utilities that focus on making z/OS management easier. Each utility is a building block to enable automation on the platform.

## Ways to contribute

There are many ways to contribute to the pyzkiln project. One way to get started is by looking at the issues section of the repository. Look for the **Good First Issue** tag. Good First Issues are great as a first contribution. 

### Adding new functionality: 
If you have a new functionality that can be added to the pyzkiln package, open a GitHub pull request with the code. In the PR, make sure to clearly document the new functionality including why it is valuable.

### Fixing bugs:
If you fix a bug, open a GitHub pull request with the fix. In the PR, make sure to clearly described the problem and the solution approach.

### Adding or fixing documentation:
If you want to improve the current documentation, that includes adding new documentation, fixing grammar, spelling, and format errors open a GitHub pull request with your changes.



## Style Guidelines

- When adding code to pyzkiln, follow the PEP8 style guide for Python
- The use of Flake8, Black, and pydocstyle as helpers is recommended

## Contribution checklist

When contributing to pyzkiln, think about the following: 

- Make any necessary updates to setup file
- Make any necessary updates to README file
- Make any necessary updates to /docs/index.md
    - Add / modify any documentation here
- Make any necessary updates to /docs/about.md
    - Add your name as a contributor, if you are not part of the list
- Add any available test cases to /tests
- Verify __init__ files are updated properly
- Run Flake8 and Black against any code modified
- Test installation of pyzkiln package after updates are made

## Found a bug?

If you find a bug in the code, please open the an issue. 
In the issue, clearly state what is the bug, and  any other details that can be helpful. 
