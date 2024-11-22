      *Copyright IBM Corp. 2024.
       IDENTIFICATION DIVISION.
       PROGRAM-ID. "COBTEST".
       DATA DIVISION.
       WORKING-STORAGE SECTION.
       01 pyrun PIC u(80) VALUE z'print("Hello, world")'.
       PROCEDURE DIVISION.
           CALL "Py_Initialize"
           CALL "PyRun_SimpleString" USING
           BY REFERENCE pyrun
           END-CALL
           CALL "Py_Finalize"

           STOP RUN.
