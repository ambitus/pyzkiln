      *Copyright IBM Corp. 2024.
       IDENTIFICATION DIVISION.
       PROGRAM-ID. "COBTEST2".
       DATA DIVISION.
       WORKING-STORAGE SECTION.
       77  PGM-NAME                   PICTURE X(13).
       LINKAGE SECTION.
       PROCEDURE DIVISION.
           DISPLAY "COBTEST2".
           MOVE "SHAREDL" to PGM-NAME.
           CALL PGM-NAME.
           GOBACK.
