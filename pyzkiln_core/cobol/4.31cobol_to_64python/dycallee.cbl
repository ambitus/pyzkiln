       IDENTIFICATION DIVISION.
       PROGRAM-ID. "DYCALLEE".
       DATA DIVISION.
       WORKING-STORAGE SECTION.
       01 pyrun PIC u(80) VALUE z'print("hello world")'.
       LINKAGE SECTION.
       PROCEDURE DIVISION.
           CALL "Py_Initialize"
           CALL "PyRun_SimpleString" USING
           BY REFERENCE pyrun
           END-CALL
           CALL "Py_Finalize"
           GOBACK.
