      *Copyright IBM Corp. 2024.
       IDENTIFICATION DIVISION.
       PROGRAM-ID. 'SHAREDL'.
       ENVIRONMENT DIVISION.
       CONFIGURATION SECTION.
       INPUT-OUTPUT SECTION.
       FILE-CONTROL.
       DATA DIVISION.
       FILE SECTION.
       WORKING-STORAGE SECTION.
       01 pyrun PIC u(80) VALUE z'import zlib; import _curses'.
       PROCEDURE DIVISION.
           DISPLAY "SHAREDLIB".
           CALL "Py_Initialize"
           CALL "PyRun_SimpleString" USING
           BY REFERENCE pyrun
           END-CALL
           CALL "Py_Finalize"
           STOP RUN.

