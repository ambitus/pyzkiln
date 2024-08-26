# set the correct path to your system compilers here
31bit_compiler= <...>
64bit_compiler= <...>

.PHONY: clean
clean:
	-rm -rf ./*.out
	-rm -rf ./*.o
	-rm -rf ./*.dll
	-rm -rf ./build
	-rm -rf ./*.egg-info
	-rm -rf ./dist
	-rm -rf ./*.so
	-rm -rf ./*.x

.PHONY: build
build:
	-$(31bit_compiler) -qnoxplink -qexportall call.c -c
	-$(31bit_compiler) -qnoxplink -qexportall asm.s -c

.PHONY: share
share:
	-$(31bit_compiler) -qdll -qexportall call.o asm.o -o CALL.dll

.PHONY: setup
setup:clean build share

.PHONY: install
install:setup
	-python setup.py build
	-python setup.py install
